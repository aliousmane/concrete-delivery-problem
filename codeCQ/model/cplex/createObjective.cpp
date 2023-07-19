#include "cplexSolver.h"

void cplexSolver::createObjective() {
    IloExpr obj(env);
    for (int i = 0; i < _graph.GetArcCount(); i++) {
        Arc *arc = _graph.GetArc(i);
        if (arc->driver == nullptr) continue;
        obj += arc->cost * x[arc->index];
    }
    for (int i = 0; i < _graph.GetOrderCount(); i++) {
        Order *o1 = _graph.GetData()->GetOrder(i);
        obj += Parameters::UNDELIVERY_PENALTY *o1->demand* (1 - o[o1->orderID]);
    }

    for (int i = 0; i < _graph.GetCustomerCount(); i++) {
        obj +=  Parameters::FIRST_DEL_PENALTY * (g[i]);
    }
    for (int k = 0; k < _graph.GetDriverCount(); k++) {
        Driver *d = _graph.GetData()->GetDriver(k);
        obj += Parameters::UNDERWORK_PENALTY * (UnderTime[d->id]);
        obj += Parameters::OVERTIME_PENALTY * (Overtime[d->id]);
    }
    obj_func = IloMinimize(env, obj);
    model.add(obj_func);
    obj.end();
}