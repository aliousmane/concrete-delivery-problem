#include "cplexSolver.h"

void cplexSolver::createVariables() {

    y = IloNumVarArray(env, _graph.GetCustomerCount(), 0, 1, ILOINT);
    g = IloNumVarArray(env, _graph.GetCustomerCount(), 0, Parameters::MAX_LATE_TW,
                       ILOFLOAT);
    o = IloNumVarArray(env, _graph.GetOrderCount(), 0, 1, ILOINT);
    x = IloNumVarArray(env, _graph.GetArcCount(), 0, 1, ILOINT);
    v = IloNumVarArray(env, _graph.GetNodeCount(), 0, 2 * Parameters::MAX_LATE_TW, ILOFLOAT);
    w = IloNumVarArray(env, _graph.GetNodeCount(), 0, 2 * Parameters::MAX_LATE_TW, ILOFLOAT);
    UnderTime = IloNumVarArray(env, _graph.GetDriverCount(), 0,
                               2 * Parameters::MAX_LATE_TW, ILOFLOAT);
    Overtime = IloNumVarArray(env, _graph.GetDriverCount(),
                              0, 2 * Parameters::MAX_LATE_TW, ILOFLOAT);

    Qjk = NumVarMatrix(env, _graph.GetDeliveryCount());
    Ud = NumVarMatrix(env, _graph.GetDeliveryCount());
    Ul = NumVarMatrix(env, _graph.GetDockCount());

    for (int i = 0; i < _graph.GetCustomerCount(); i++) {
        Customer *c = _graph.GetData()->GetCustomer(i);
        char name[15];
        snprintf(name, 15, "y%d", c->constID);
        y[c->custID].setName(name);
        y[c->custID].setBounds(0, 1);
        snprintf(name, 15, "g%d", c->constID);
        g[c->custID].setName(name);
        model.add(g[c->custID]>=0);

        std::vector<Order *> c_orders = _graph.GetData()->GetOrders(c);
        for (auto o1: c_orders) {
            snprintf(name, 15, "o%d_%d", o1->orderID, c->constID);
            o[o1->orderID].setName(name);
            o[o1->orderID].setBounds(0, 1);
        }
    }

    for (int i = 0; i < _graph.GetNodeCount(); i++) {
        Node *n = _graph.GetData()->GetNode(i);
        char name[15];
        snprintf(name, 15, "v%c_%d", n->c, n->id);
        v[n->id].setName(name);
//        v[n->id].setBounds(n->early_tw, n->late_tw);
        snprintf(name, 15, "w%c_%d", n->c, n->id);
        w[n->id].setName(name);
//        w[n->id].setBounds(n->early_tw, n->late_tw);
        model.add(w[n->id] >= v[n->id]);

        if (n->type == Parameters::DELIVERY) {
            auto del = dynamic_cast<Delivery *>(n);
            Qjk[del->delID] = IloNumVarArray(env, _graph.GetDriverCount());
            Ud[del->delID] = IloNumVarArray(env, _graph.GetDeliveryCount());
            for (int k = 0; k < _graph.GetDriverCount(); ++k) {
                Driver *d = _graph.GetDriver(k);
                snprintf(name, 15, "Q%d_%d", del->id, k);
                Qjk[del->delID][k] = IloNumVar(env, 0, d->capacity, ILOFLOAT);
                Qjk[del->delID][k].setName(name);
//                model.add(Qjk[del->delID][k] >=0);
                model.add(Qjk[del->delID][k] <= d->capacity); //! c35
            }

            for (int k = 0; k < _graph.GetDeliveryCount(); ++k) {
                Delivery *next_del = _graph.GetDelivery(k);
                if (next_del->id == del->id) continue;
                if (next_del->custID != del->custID)
                    continue;
                //TODO check if deliveries have links

                Ud[del->delID][next_del->delID] = IloNumVar(env, 0, 1, ILOINT);
                snprintf(name, 15, "Ud%d_%d", del->id, next_del->id);
                Ud[del->delID][next_del->delID].setName(name);
            }

        }
        if (n->type == Parameters::DOCK) {
            v[n->id].setBounds(0, n->late_tw);
            w[n->id].setBounds(0, n->late_tw);
            auto dock = dynamic_cast<Dock *>(n);
            Ul[dock->dockID] = IloNumVarArray(env, _graph.GetDockCount());

            for (int k = 0; k < _graph.GetDockCount(); ++k) {
                Dock *next_dock = _graph.GetDock(k);
                if (next_dock->id == dock->id) continue;

                if (next_dock->depotID != dock->depotID)
                    continue;

                if (next_dock->orderID == dock->orderID) {
                    if (next_dock->rank < dock->rank)
                        continue;
                }
                //TODO check if docks have links
                link_dock_out[dock->dockID].insert(next_dock->dockID);
                link_dock_in[next_dock->dockID].insert(dock->dockID);
                Ul[dock->dockID][next_dock->dockID] = IloNumVar(env, 0, 1, ILOINT);
                snprintf(name, 15, "Ul%d_%d", dock->id, next_dock->id);
                Ul[dock->dockID][next_dock->dockID].setName(name);
            }

        }
    }

    for (int i = 0; i < _graph.GetArcCount(); i++) {
        Arc *arc = _graph.GetArc(i);
        char name[15];
        if (arc->driver == nullptr) {
            snprintf(name, 15, "x%c%d_%c%d_(-1)", arc->from->c,
                     arc->from->id, arc->to->c, arc->to->id);
            model.add(x[i] == 0);
        } else {
            snprintf(name, 15, "x%c%d_%c%d_%d", arc->from->c,
                     arc->from->id, arc->to->c, arc->to->id, arc->driver->id);
            x[i].setName(name);
        }
    }

    for (int k = 0; k < _graph.GetDriverCount(); ++k) {
        Driver *d = _graph.GetDriver(k);
        char name[15];
        snprintf(name, 15, "less_%d", d->id);
        UnderTime[k].setName(name);
        snprintf(name, 15, "over_%d", d->id);
        Overtime[k].setName(name);
    }

}