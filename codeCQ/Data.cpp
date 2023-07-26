
#include "Data.h"
#include "Parameters.h"
#include <fstream>
#include <cassert>
#include <iostream>

using namespace std;


Data::Data()
        : input(), output(), nbDepots(0), nbCustomers(0), nbOrders(0), nbDrivers(0), _depots(0), _customers(0),
          driverCapacities(),
          maxDriverCap(0), minDriverCap(0), _deliveries(0), _drivers(0), _orders_deliveries(0), _distances(0),
          _times(0), _docks(0),
          _index_orders(0),
//          path(std::filesystem::current_path().parent_path().parent_path()),
          path(std::filesystem::current_path().parent_path()),
          instance_name(),
          problem_name(), sol_output(), result_file() {
    time_mat_file = path + "/matrices/Matrice_de_temps_all.csv";
    distance_mat_file = path + "/matrices/Matrice_de_distance_all.csv";
}

void Data::Load() {
    if (Parameters::KINABLE) {
        Parameters::ADJUSTMENT_DURATION = 0;
        Parameters::CLEANING_DURATION = 0;
        LoadKinableInstance();
    } else {
//        LoadInstance();
        LoadCQInstance();
        //Data::LoadMatrices(_distances,distance_mat_file,1);
        Data::LoadMatrices(_times, time_mat_file, 60);
        Data::LoadMatrices(_distances, distance_mat_file, 1);
    }
    AddDeliveryNodes();
    AddDockNodes();
}

void Data::LoadInstance() {

    std::ifstream inputFile(this->input);
    if (inputFile.is_open()) {
        string dummy;
        inputFile >> dummy >> this->nbDepots;
        inputFile >> dummy >> this->nbCustomers;
        inputFile >> dummy >> this->nbOrders;
        inputFile >> dummy >> this->nbDrivers;

        for (int i = 0; i < this->nbDepots; i++) {
            Depot n;
            inputFile >> n.depotID >> n.distID >> n.depotLoc >> n.capacity;
            n.id = GetNodeCount();
            n.no = n.id + 1;
            n.depotID = i;
            n.SetTW(0, Parameters::MAX_LATE_TW);
            n.type = Parameters::DEPOT;
            Node d1;
            d1.id = n.id + 1;
            d1.no = d1.id + 1;
            d1.distID = n.distID;
            d1.type = Parameters::START_LINK;
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            n.StartNodeID = d1.id;
            n.EndNodeID = d2.id;
            AddDepot(n);

            AddNode(d1);
            AddNode(d2);
        }

        minEarlyTW = 1440;
        maxEarlyTW = 0;
        minLateTW = 1440;
        maxLateTW = 0;
        MinDemand = INFINI;
        MaxDemand = 0;
        SumDemand = 0;
        for (int i = 0; i < this->nbCustomers; ++i) {
            Customer n;
            n.c = 'C';
            inputFile >> n.custID >> n.orderNbr >> dummy >> n.distID >> n.depotID >> n.early_tw >> n.demand
                      >> n.nbOrder;
            n.custID = i;
            n.constID = n.custID;
            n.id = GetNodeCount();
            n.no = n.id + 1;
//            n.late_tw = n.early_tw + ceil(UnloadingTime(n.demand, n.demand)) + 60;
            n.late_tw = 2 * Parameters::MAX_LATE_TW;
            n.type = Parameters::CUSTOMER;
            Node d1;
            d1.id = n.id + 1;
            d1.no = d1.id + 1;
            d1.distID = n.distID;
            d1.type = Parameters::START_LINK;
            d1.c = 'S';
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            d2.c = 'E';
            n.StartNodeID = d1.id;
            n.EndNodeID = d2.id;

            AddCustomer(n);
            AddNode(d1);
            AddNode(d2);

            SumDemand += n.demand;
            MinDemand = std::min(MinDemand, n.demand);
            MaxDemand = std::max(MaxDemand, n.demand);

            minEarlyTW = std::min(minEarlyTW, n.early_tw);
            maxEarlyTW = std::max(maxEarlyTW, n.early_tw);
            minLateTW = std::min(minLateTW, n.early_tw);
            maxLateTW = std::max(maxLateTW, n.early_tw);
        }

        for (int i = 0; i < this->nbOrders; ++i) {
            Order o;
            inputFile >> o.orderID >> o.custID >> o.demand;
            o.orderID = GetOrderCount();
            AddOrder(o);
        }
        this->driverCapacities.clear();
        for (int i = 0; i < this->nbDrivers; i++) {
            Driver d;
            inputFile >> d.id >> d.no >> d.truck_nbr >> d.rank >> d.depotID >>
                      d.capacity >> d.start_shift_time;
            d.id = i;
            if (d.rank == -1) d.rank = MAX_RANK;
//            d.start_shift_time = 260;
            Depot *dep = GetDepot(d.depotID);
            d.distID = dep->distID;

            Node d1;
            d1.id = GetNodeCount();
            d1.no = d1.id + 1;
            d1.distID = dep->distID;
            d1.type = Parameters::START_LINK;
            d1.c = 'S';
            d1.early_tw = d.start_shift_time;
            d1.late_tw = d.start_shift_time + Parameters::MAX_WORKING_TIME;
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d.StartNodeID = d1.id;
            d.EndNodeID = d2.id;
            d2.c = 'E';
            AddNode(d1);
            AddNode(d2);
            AddDriver(d);
            this->driverCapacities.insert(d.capacity);
        }
        this->minDriverCap = *this->driverCapacities.begin();
        this->maxDriverCap = *this->driverCapacities.rbegin();
    } else {
        cout << "Erreur ouverture " << this->input << endl;
        exit(1);
    }
}

void Data::LoadCQInstance() {
    std::ifstream inputFile(this->input);
    if (inputFile.is_open()) {
        string dummy;
        inputFile >> dummy >> this->nbDrivers;
        for (int i = 0; i < this->nbDrivers; i++) {
            Driver d;
            inputFile >> d.id >> d.no >> d.truck_nbr >> d.rank >> d.depotID >>
                      d.capacity >> d.start_shift_time;
            d.id = i;
            if (d.rank == -1) d.rank = MAX_RANK;
            d.start_shift_time = 260;
            Node d1;
            d1.id = GetNodeCount();
            d1.no = d1.id + 1;
            d1.type = Parameters::START_LINK;
            d1.c = 'S';
            d1.early_tw = d.start_shift_time;
            d1.late_tw = d.start_shift_time + Parameters::MAX_WORKING_TIME;
            d1.siteID = d1.id;
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d.StartNodeID = d1.id;
            d.EndNodeID = d2.id;
            d2.siteID = d2.id;
            d2.c = 'E';
            AddNode(d1);
            AddNode(d2);
            AddDriver(d);
            this->driverCapacities.insert(d.capacity);
        }
        this->minDriverCap = *this->driverCapacities.begin();
        this->maxDriverCap = *this->driverCapacities.rbegin();


        minEarlyTW = 1440;
        maxEarlyTW = 0;
        minLateTW = 1440;
        maxLateTW = 0;
        MinDemand = INFINI;
        MaxDemand = 0;
        SumDemand = 0;
        inputFile >> dummy >> this->nbCustomers >> dummy;

        for (int i = 0; i < this->nbCustomers; ++i) {
            Customer n;
            n.c = 'C';
            inputFile >> n.custID >> n.orderNbr >> dummy >> n.distID >> n.depotID >> n.early_tw >> n.demand
                      >> n.nbOrder;
            n.custID = i;
            n.constID = n.custID;
            n.id = GetNodeCount();
            n.no = n.id + 1;
//            n.late_tw = n.early_tw + ceil(UnloadingTime(n.demand, n.demand)) + 60;
            n.late_tw = n.early_tw + Parameters::MAX_LATE_TW;
            n.type = Parameters::CUSTOMER;
            n.siteID = n.id;
            Node d1;
            d1.id = n.id + 1;
            d1.no = d1.id + 1;
            d1.distID = n.distID;
            d1.type = Parameters::START_LINK;
            d1.siteID = d1.id;
            d1.c = 'S';
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            d2.c = 'E';
            n.StartNodeID = d1.id;
            n.EndNodeID = d2.id;
            d2.siteID = d2.id;

            AddCustomer(n);
            AddNode(d1);
            AddNode(d2);

            SumDemand += n.demand;
            MinDemand = std::min(MinDemand, n.demand);
            MaxDemand = std::max(MaxDemand, n.demand);

            minEarlyTW = std::min(minEarlyTW, n.early_tw);
            maxEarlyTW = std::max(maxEarlyTW, n.early_tw);
            minLateTW = std::min(minLateTW, n.early_tw);
            maxLateTW = std::max(maxLateTW, n.early_tw);
        }
        inputFile >> dummy >> this->nbOrders;

        for (int i = 0; i < this->nbOrders; ++i) {
            Order o;
            inputFile >> o.orderID >> o.custID >> o.demand;
            o.orderID = GetOrderCount();
            AddOrder(o);
        }
        inputFile >> dummy >> this->nbDepots;
        for (int i = 0; i < this->nbDepots; i++) {
            Depot n;
            inputFile >> n.depotID >> n.distID >> n.depotLoc >> n.capacity;
            n.id = GetNodeCount();
            n.no = n.id + 1;
            n.depotID = i;
            n.SetTW(0, Parameters::MAX_LATE_TW);
            n.type = Parameters::DEPOT;
            n.siteID = n.id;

            Node d1;
            d1.id = n.id + 1;
            d1.no = d1.id + 1;
            d1.distID = n.distID;
            d1.type = Parameters::START_LINK;
            d1.siteID = d1.id;

            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            n.StartNodeID = d1.id;
            n.EndNodeID = d2.id;
            d2.siteID = d2.id;

            AddDepot(n);

            AddNode(d1);
            AddNode(d2);
        }

        for (int i = 0; i < this->nbDrivers; i++) {
            Driver *d = GetDriver(i);
            Depot *dep = GetDepot(d->depotID);
            d->distID = dep->distID;

            Node *d1 = GetNode(d->StartNodeID);
            Node *d2 = GetNode(d->EndNodeID);

            d1->distID = dep->distID;
            d2->distID = dep->distID;
        }

    } else {
        cout << this->input << " not found\n";
        exit(1);
    }
}

void Data::LoadKinableInstance() {
    std::ifstream inputFile(this->input);
    if (inputFile.is_open()) {
        string dummy;
        inputFile >> dummy >> Parameters::INTRA_ORDER_DELIVERY;
        inputFile >> dummy >> this->nbDrivers;
        this->driverCapacities.clear();
        for (int i = 0; i < this->nbDrivers; i++) {
            Driver d;
            inputFile >> dummy >> d.capacity >> d.serviceDuration;
            d.id = i;
            d.no = d.id + 1;
            d.rank = 1;
            AddDriver(d);
            this->driverCapacities.insert(d.capacity);
        }
        this->minDriverCap = *this->driverCapacities.begin();
        this->maxDriverCap = *this->driverCapacities.rbegin();

        inputFile >> dummy >> this->nbCustomers;
        minEarlyTW = 1440;
        maxEarlyTW = 0;
        minLateTW = 1440;
        maxLateTW = 0;
        MinDemand = INFINI;
        MaxDemand = 0;
        SumDemand = 0;
        for (int i = 0; i < this->nbCustomers; ++i) {
            Customer n;
            n.c = 'C';
            inputFile >> dummy >> n.demand >> n.early_tw >> n.late_tw;
            n.custID = GetCustomerCount();
            n.constID = n.custID;
            n.id = GetNodeCount();
            n.no = n.id + 1;
            n.distID = n.id;
            n.type = Parameters::CUSTOMER;
            n.orderNbr = GetOrderCount();
            n.nbOrder = 1;
            AddCustomer(n);
            SumDemand += n.demand;
            MinDemand = std::min(MinDemand, n.demand);
            MaxDemand = std::max(MaxDemand, n.demand);

            minEarlyTW = std::min(minEarlyTW, n.early_tw);
            maxEarlyTW = std::max(maxEarlyTW, n.early_tw);
            minLateTW = std::min(minLateTW, n.early_tw);
            maxLateTW = std::max(maxLateTW, n.early_tw);

            Order o;
            o.orderID = GetOrderCount();
            o.custID = n.custID;
            o.demand = n.demand;
            o.early_tw = n.early_tw;
            o.late_tw = n.late_tw;
            AddOrder(o);
        }
        this->nbOrders = this->nbCustomers;

        inputFile >> dummy >> this->nbDepots;
        for (int i = 0; i < this->nbDepots; i++) {
            inputFile >> dummy;
        }
        inputFile >> dummy >> dummy;

        Node startDepot;
        inputFile >> dummy >> startDepot.x >> startDepot.y;
        Node endDepot;
        inputFile >> dummy >> endDepot.x >> endDepot.y;

        for (int i = 0; i < this->nbDepots; i++) {
            Depot n;
            inputFile >> dummy >> n.x >> n.y;
            n.id = GetNodeCount();
            n.no = n.id + 1;
            n.distID = n.id;
            n.depotID = i;
            n.type = Parameters::DEPOT;
            Node d1;
            d1.id = n.id + 1;
            d1.no = d1.id + 1;
            d1.distID = n.distID;
            d1.type = Parameters::START_LINK;
            d1.x = n.x;
            d1.y = n.y;
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            n.StartNodeID = d1.id;
            n.EndNodeID = d2.id;
            AddDepot(n);

            AddNode(d1);
            AddNode(d2);


        }
        for (int i = 0; i < GetCustomerCount(); i++) {
            Customer *c = GetCustomer(i);
            inputFile >> dummy >> c->x >> c->y;
            Node d1;
            d1.id = GetNodeCount();
            d1.no = d1.id + 1;
            d1.distID = c->distID;
            d1.type = Parameters::START_LINK;
            d1.x = c->x;
            d1.y = c->y;
            d1.c = 'S';
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            d2.c = 'E';
            c->StartNodeID = d1.id;
            c->EndNodeID = d2.id;

            AddNode(d1);
            AddNode(d2);
        }
        const int driver_distID = GetNodeCount();
        for (int i = 0; i < GetDriverCount(); i++) {
            Driver *d = GetDriver(i);
            d->distID = driver_distID;
            Node d1;
            d1.id = GetNodeCount();
            d1.no = d1.id + 1;
            d1.distID = d->distID;
            d1.type = Parameters::START_LINK;
            d1.x = startDepot.x;
            d1.y = startDepot.y;
            d1.c = 'S';
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d->StartNodeID = d1.id;
            d->EndNodeID = d2.id;
            d2.c = 'E';
            AddNode(d1);
            AddNode(d2);
        }


        const int dim = driver_distID + 1;
        std::vector<double> minDistance(GetNodeCount(), 100000);
        std::vector<int> flags(dim, -1);
        _times.resize(dim);
        _distances.resize(dim);
        for (int i = 0; i < dim; ++i) {
            _times[i].resize(dim, 0);
            _distances[i].resize(dim, 0);
        }

        for (int i = 0; i < GetNodeCount(); i++) {
            Node *ni = GetNode(i);
//            if(ni->type==Parameters::NODE) continue;
            if (flags[ni->distID] != -1) {
                continue;
            }
            flags[ni->distID] = ni->distID;
//            cout<<*ni <<" ni "<< ni->distID<< endl;
            for (int j = 0; j < GetNodeCount(); j++) {
                auto *nj = GetNode(j);
                if (ni->distID == nj->distID) continue;

                _times[ni->distID][nj->distID] =
                        std::ceil(sqrt(pow((ni->x - nj->x), 2) + pow((ni->y - nj->y), 2)));
                _distances[ni->distID][nj->distID] = _times[ni->distID][nj->distID];
                _times[nj->distID][ni->distID] = _times[ni->distID][nj->distID];
                _distances[nj->distID][ni->distID] = _distances[ni->distID][nj->distID];
//                if(ni->id==11) cout<<*nj <<" nj "<< nj->distID<< endl;

                // Assign nearest depot to customer
                if (ni->type == Parameters::DEPOT && nj->type == Parameters::CUSTOMER) {
                    if (_times[ni->distID][nj->distID] < minDistance[nj->id]) {
                        nj->depotID = ni->depotID;
                        minDistance[nj->id] = _times[ni->distID][nj->distID];
//                        cout<<ni->distID<<" "<<nj->distID << " "<< minDistance[nj->id]<<endl;

//                        cout<<nj->x<<" "<<nj->y<<endl;
                    }
                }
            }
        }
//        cout<<Travel(15,12)<<endl;
//        exit(1);
    } else {
        cout << "Erreur ouverture " << endl;
        exit(1);
    }
}


void Data::AddNode(const Node &n) {
    _nodes.push_back(std::make_shared<Node>(n));
}

void Data::AddDepot(const Depot &n) {
    _depots.push_back(GetNodeCount());
    _nodes.push_back(std::make_shared<Depot>(n));
}

void Data::AddDelivery(const Delivery &del) {
    _deliveries.push_back(GetNodeCount());
    _orders_deliveries[del.orderID].push_back(GetNodeCount());
    _nodes.push_back(std::make_shared<Delivery>(del));
}

void Data::AddCustomer(const Customer &n) {
    _customers.push_back(GetNodeCount());
    _index_orders.emplace_back();
    _nodes.push_back(std::make_shared<Customer>(n));
}

void Data::AddDock(const Dock &n) {
    _docks.push_back(GetNodeCount());
    _nodes.push_back(std::make_shared<Dock>(n));
}


Depot *Data::GetDepot(int i) {
    return dynamic_pointer_cast<Depot>(_nodes[_depots[i]]).get();
}

Depot *Data::GetDepot(Node *n) {
    return GetDepot(n->depotID);
}

Customer *Data::GetCustomer(int i) const {
    return dynamic_pointer_cast<Customer>(_nodes[_customers[i]]).get();
}

void Data::AddOrder(Order &o) {
    _index_orders[o.custID].emplace_back(GetOrderCount());
    _orders.emplace_back(o);
    _orders_deliveries.emplace_back();
}

Order *Data::GetOrder(Customer *c, int index) {
    return &_orders[_index_orders[c->custID][index]];
}

Order *Data::GetOrder(int index) {
    return &_orders[index];
}

std::vector<Order *> Data::GetOrders(Customer *c) {
    std::vector<Order *> temp;
    for (auto i: _index_orders[c->custID])
        temp.emplace_back(&_orders[i]);
    return temp;
}

Delivery *Data::GetDelivery(Order *o, int index) {
    if (index < 0) return nullptr;
    if (index >= GetDeliveryCount(o)) return nullptr;
    return dynamic_pointer_cast<Delivery>(_nodes[_orders_deliveries[o->orderID][index]]).get();
}

Dock *Data::GetDock(Order *o, int index) {
    if (index < 0) return nullptr;
    if (index >= GetDeliveryCount(o)) return nullptr;
    Delivery *del = GetDelivery(o, index);
    return GetDock(del->dockID);
}


Delivery *Data::GetDelivery(int index) {
    return dynamic_pointer_cast<Delivery>(_nodes[_deliveries[index]]).get();
}

Dock *Data::GetDock(int dockID) {
    return dynamic_pointer_cast<Dock>(_nodes[_docks[dockID]]).get();
}

void Data::AddDriver(Driver &d) {
    _drivers.emplace_back(d);
}

Driver *Data::GetDriver(int id) {
    return &_drivers[id];
}

void Data::AddDeliveryNodes() {
    for (int i = 0; i < _customers.size(); i++) {
        Customer *c = GetCustomer(i);
        AddDeliveryNodes(c);
    }
}

void Data::AddDeliveryNodes(Customer *c) {
    for (int i = 0; i < _index_orders[c->custID].size(); i++) {
        Order *o1 = GetOrder(c, i);
        o1->early_tw = c->early_tw;
        o1->late_tw = c->late_tw;
        const short nb = ceil(double(o1->demand) / minDriverCap);
        AddDeliveryNodes(o1, nb);
        o1->nbDelMax = nb;
        o1->nbDelMin = ceil(double(o1->demand) / maxDriverCap);
    }
}

void Data::AddDeliveryNodes(Order *o, int nb) {
    Customer *c = GetCustomer(o->custID);
    assert(c->depotID != -1);
    for (int j = 0; j < nb; ++j) {
        Delivery del;
        del.id = GetNodeCount();
        del.delID = GetDeliveryCount();
        del.no = del.id + 1;
        del.rank = j;
        del.orderID = o->orderID;
        del.custID = o->custID;
        del.type = Parameters::DELIVERY;
        del.StartNodeID = c->StartNodeID;
        del.EndNodeID = c->EndNodeID;
        del.distID = c->distID;
        del.early_tw = c->early_tw;
        del.late_tw = c->late_tw;
        del.c = 'D';
        del.siteID = c->siteID;
        AddDelivery(del);
    }
}

void Data::AddDockNodes() {

    for (int i = 0; i < GetDeliveryCount(); ++i) {
        Delivery *del = GetDelivery(i);
        Customer *c = GetCustomer(del->custID);
        Depot *dep = GetDepot(c->depotID);
        del->depotID = c->depotID;
        Dock d1;
        d1.id = GetNodeCount();
        d1.dockID = GetDockCount();
        d1.custID = c->custID;
        d1.no = d1.id + 1;
        d1.type = Parameters::DOCK;
        d1.delID = del->delID;
        d1.rank = del->rank;
        del->dockID = d1.dockID;
        d1.depotID = del->depotID;
        d1.distID = dep->distID;
        d1.orderID = del->orderID;
        d1.early_tw = del->early_tw;
        d1.late_tw = del->late_tw;
        d1.c = 'L';
        d1.siteID = c->siteID;
        AddDock(d1);
    }
}

void Data::LoadMatrices(std::vector<std::vector<double>> &array, const string &matrix_filename, const int rate) {
    string line;
    ifstream f(matrix_filename.c_str()); // open file
    if (!f.is_open()) {
        perror(("error while opening file " + matrix_filename).c_str());
        exit(1);
    }

    while (getline(f, line)) {                       // read each line
        string val;           // string to hold value
        vector<double> row;   // vector for row of values
        stringstream s(line); // stringstream to parse csv

        while (getline(s, val, ',')) // for each value
        {
            row.push_back(stod(val) / rate);
        }
        array.push_back(row);
    }
    f.close();
}


void Data::ShowData() {
    cout << "Depots" << endl;
    for (int i = 0; i < _depots.size(); i++) {
        cout << *GetDepot(i) << endl;
    }
    cout << "Customers" << endl;
    for (int i = 0; i < _customers.size(); i++) {
        Customer *c = GetCustomer(i);
        cout << *c << endl;
        cout << "Orders" << endl;
        for (int j = 0; j < _index_orders[c->custID].size(); j++) {
            Order *o = GetOrder(c, j);
            cout << *o << endl;
            cout << "Deliveries" << endl;
            for (int k = 0; k < _orders_deliveries[o->orderID].size(); k++) {
                Delivery *del = GetDelivery(o, k);
                cout << *del << endl;
                Dock *dock = GetDock(del->dockID);
                cout << *dock << endl;
            }
        }
    }

    cout << "Drivers" << endl;
    for (auto &d: _drivers) {
        cout << d << endl;
    }
}



