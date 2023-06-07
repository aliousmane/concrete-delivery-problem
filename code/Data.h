
#ifndef CODE_DATA_H
#define CODE_DATA_H
#include "Node.h"
#include "Driver.h"
#include "Parameters.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <sstream>
#include <filesystem>
class Data {
public:
    Data() : input(), output(), nbDepots(0), nbCustomers(0), nbOrders(0), nbDrivers(0), _depots(0), _customers(0), driverCapacities(),
              maxDriverCap(0), minDriverCap(0), _deliveries(0), _drivers(0), _orders_deliveries(0), _distances(0), _times(0), _docks(0),
              _index_orders(0),path(std::filesystem::current_path().parent_path()),instance_name(),
              problem_name(),sol_output(),result_file()
    {
        time_mat_file= "../../matrices/Matrice_de_temps_all.csv";
        distance_mat_file= "../../matrices/Matrice_de_distance_all.csv";
    }
    std::string input;
    std::string output;
    std::string sol_output;
    std::string instance_name;
    std::string result_file;
    std::string problem_name;
    std::string path;
    int nbDepots,nbCustomers, nbOrders, nbDrivers, maxDriverCap, minDriverCap;
    std::set<int> driverCapacities;
    int minEarlyTW {0};
    int minLateTW {0};
    int maxEarlyTW {0};
    int maxLateTW {0};
    int MinDemand{0};
    int MaxDemand{0};
    int SumDemand{0};
    void GetParams(int argc, const char **argv){
        if(argc>1)
            input = argv[1];
        if(argc >2)
            Parameters::RUNTIME = strtol(argv[2], nullptr,10);

        result_file = path + "/result/" + problem_name + "/results.csv";
        if(argc>3)
            result_file = path + "/result/" + problem_name +"/"+  argv[3];

        instance_name = std::filesystem::path(input).stem();
        problem_name = std::filesystem::path(input).parent_path().filename();
        sol_output = path + "/solution/" + problem_name + "/" + instance_name + ".csv";
    }
    void Load();
    void LoadInstance();
    void LoadKinableInstance();
    int GetNodeCount()const{return int(_nodes.size());}
    int GetOrderCount()const{return int(_orders.size());}
    int GetDepotCount()const{return int(_depots.size());}
    int GetCustomerCount()const{return int(_customers.size());}
    void AddNode(const Node &n);
    void AddDock(const Dock &n);
    void AddDepot(const Depot &n);
    void AddOrder( Order &o);
    void AddDelivery( const Delivery &del);
    void AddCustomer(const Customer &n);
    void AddDeliveryNodes(Customer *c);
    void AddDeliveryNodes();
    void AddDeliveryNodes(Order *o,int nb);
    void AddDockNodes();
    void AddDriver(Driver &d);
    Driver * GetDriver(int id);
    Depot * GetDepot(int i);
    Depot * GetDepot(Node *n);
    Node * GetNode(int i){ return _nodes[i].get();}
    Customer * GetCustomer(int i) const;
    Order * GetOrder(Customer *c, int index);
    Order * GetOrder(int index);
    Delivery *GetDelivery(Order *o,int index);
    Delivery *GetDelivery(int index);
    Dock *GetDock(int dockID);
    int GetDeliveryCount()const{return int(_deliveries.size());}
    int GetDeliveryCount(int ordID)const{return (int)_orders_deliveries[ordID].size();  }
    int GetDeliveryCount(Order *o)const{return (int)_orders_deliveries[o->orderID].size();  }
    int GetDockCount()const{return int(_docks.size());}
    int GetDriverCount()const{return int(_drivers.size());}

    std::vector<Order*>  GetOrders(Customer *c);

    [[maybe_unused]] void ShowData();
    int Travel(Node *from, Node *to) {
        return  ceil(_times[from->distID][to->distID]);
    }
    int Travel(Customer *from, Customer *to) {
        Depot * dep_to = GetDepot(to->depotID);
        return  Travel(from,dep_to) + Travel(dep_to,to) ;
    }
    int Travel(int from, int to) {
        return ceil(_times[from][to]);
    }
    static int LoadingTime(Depot *dep, int load) {
        if(Parameters::KINABLE)
            return 0;
        return std::ceil(double(60 * load) / dep->capacity);
    }
    static int UnloadingTime(Delivery *del, int demand, Driver *d) {
        if(Parameters::KINABLE)
            return d->serviceDuration;

        return std::ceil(double(60 * demand) / Parameters::UNLOADING_RATE);
    }
    static int UnloadingTime( int demand, int serviceDuration) {
        if(Parameters::KINABLE)
            return serviceDuration;

        return std::ceil(double(demand) / Parameters::UNLOADING_RATE);
    }
    static int CleaningTime( Delivery *del,Driver *d) {
        return Parameters::CLEANING_DURATION;
    }

    int EarlyTW(Delivery *n) const { return GetCustomer(n->custID)->early_tw; }
    int EarlyTW(int custID) const { return GetCustomer(custID)->early_tw; }
    int LateTW(Delivery *n) const { return GetCustomer(n->custID)->late_tw; }
    int LateTW(int custID) const { return GetCustomer(custID)->late_tw; }

    template<typename T>
    Data copyCustomersData(T const &listCustId) {
        Data temp;
        std::set<int> depotIds;

        for(auto custId:listCustId){
            Customer *c = GetCustomer(custId);
            Customer cur_c = Customer(*c);
            cur_c.id = temp.GetNodeCount();
            cur_c.no = cur_c.id+1;
            cur_c.custID = temp.GetCustomerCount();
            Node d1;
            d1.id = cur_c.id + 1;
            d1.no = d1.id + 1;
            d1.distID = cur_c.distID;
            d1.type = Parameters::START_LINK;
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d2.nodeID = d2.id;
            cur_c.StartNodeID = d1.id;
            cur_c.EndNodeID = d2.id;

            temp.AddCustomer(cur_c);
            temp.AddNode(d1);
            temp.AddNode(d2);

            temp.SumDemand += cur_c.demand;
            temp.MinDemand = std::min(temp.MinDemand,(int)cur_c.demand);
            temp.MaxDemand = std::max(temp.MaxDemand,(int)cur_c.demand);

            temp.minEarlyTW = std::min(temp.minEarlyTW,cur_c.early_tw);
            temp.maxEarlyTW = std::max(temp.maxEarlyTW,cur_c.early_tw);
            temp.minLateTW = std::min(temp.minLateTW,cur_c.early_tw);
            temp.maxLateTW = std::max(temp.maxLateTW,cur_c.early_tw);

            temp.nbCustomers++;
            for(auto cur_order: GetOrders(c)){
                Order o(*cur_order);
                o.orderID = temp.GetOrderCount();
                o.custID = cur_c.custID;
                temp.AddOrder(o);
                temp.nbOrders++;
            }
            depotIds.insert(c->depotID);
        }
        std::map<int,int> oldDepot_depot;
        for(auto depId: depotIds){
            Depot *dep = GetDepot(depId);
            Depot n(*dep);
            n.id = temp.GetNodeCount();
            n.no = n.id+1;
            n.depotID = temp.GetDepotCount();

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
            temp.AddDepot(n);

            temp.AddNode(d1);
            temp.AddNode(d2);

            oldDepot_depot[dep->depotID]=n.depotID;
            temp.nbDepots++;
        }

        for(int i=0;i<temp.GetCustomerCount();i++){
            Customer *c = temp.GetCustomer(i);
            c->depotID = oldDepot_depot[c->depotID];
        }

        temp.driverCapacities.clear();
        for (int i = 0; i < GetDriverCount(); i++){
            Driver *old_d = GetDriver(i);

            Driver d(*old_d);
            d.depotID = oldDepot_depot[d.depotID];
            Node d1;
            d1.id = temp.GetNodeCount();
            d1.no = d1.id + 1;
            d1.distID = d.distID;
            d1.type = Parameters::START_LINK;
            Node d2 = d1;
            d2.id = d1.id + 1;
            d2.no = d2.id + 1;
            d2.type = Parameters::END_LINK;
            d.StartNodeID = d1.id;
            d.EndNodeID = d2.id;
            temp.AddNode(d1);
            temp.AddNode(d2);
            temp.AddDriver(d);
            temp.driverCapacities.insert(d.capacity);
        }
        temp.minDriverCap = *temp.driverCapacities.begin();
        temp.maxDriverCap = *temp.driverCapacities.rbegin();
        temp.nbDrivers = this->nbDrivers;
        temp.AddDeliveryNodes();
        temp.AddDockNodes();
        temp._distances = this->_distances;
        temp._times = this->_times;
        return temp;
    }
    std::vector<Customer*> GetCustomers() const{
        std::vector<Customer*> list_cust(GetCustomerCount());
        for(int i=0;i<GetCustomerCount();i++){
            list_cust[i]= GetCustomer(i);
        }
        return list_cust;
    }
    std::vector<Driver*> GetDrivers(int cap){
        std::vector<Driver*> curDrivers;
            for(int i=0;i<GetDriverCount();i++){
                Driver *d = GetDriver(i);
                if(d->capacity == cap)
                    curDrivers.push_back(d);
            }
        return curDrivers;
    }
    std::vector<Driver*> GetDrivers(){
        std::vector<Driver*> curDrivers(GetDriverCount());
        for(int i=0;i<GetDriverCount();i++){
            curDrivers[i]=GetDriver(i);
        }
        return curDrivers;
    }
    std::set<int> GetDriversId() const{
        std::set<int> curDrivers;
        for(int i=0;i<GetDriverCount();i++){
            curDrivers.insert(i);
        }
        return curDrivers;
    }


private:
    std::vector<int> _depots;
    std::vector<int> _customers;
    std::vector<int> _deliveries;
    std::vector<int> _docks;
    std::vector<Driver> _drivers;
    std::vector< std::vector< int>> _orders_deliveries;
    std::vector<Order> _orders;
    std::vector<std::vector<int>> _index_orders;
    std::vector<std::shared_ptr<Node>> _nodes;
    std::string time_mat_file, distance_mat_file;
    static void LoadMatrices(std::vector<std::vector<double>> &array, const std::string &matrix_filename,int rate);

    std::vector<std::vector<double>> _distances;
    std::vector<std::vector<double>> _times;
};


#endif //CODE_DATA_H
