#include "cplexSolver.h"
using namespace std;
void cplexSolver::solveModel() {
    long time = Parameters::GetElapsedTime();
    bool re = cplex.solve();
    int cplex_status = (int) cplex.getCplexStatus();
    double sol = re ? cplex.getObjValue() : 0;
    time = Parameters::GetElapsedTime() - time;
    printf("re:%d sol:%lf status:%d nbnodes:%d time:%.3ld\n", (int) re, sol,
           cplex_status, (int) cplex.getNnodes(), time);
    cplex.exportModel("cplex.lp");
    cplex.writeSolution("cplex.sol");

    for(int i=0;i<_graph.GetDeliveryCount();i++){
        Delivery *del = _graph.GetDelivery(i);
        Dock *dock = _graph.GetDock(del->dockID);
        env.out()<<*del<<"\n";
//        env.out()<<*dock<<"\n";
        env.out()<<cplex.getValue(v[dock->id])<<"-"<<cplex.getValue(w[dock->id])<<" ";
        env.out()<<cplex.getValue(v[del->id])<<"-"<<cplex.getValue(w[del->id])<<" ";
        for(int k=0;k<_graph.GetDriverCount();k++) {
            Driver *d = _graph.GetDriver(k);
            if(cplex.getValue(Qjk[del->delID][d->id])<EPS) continue;
            env.out()<<" "<<cplex.getValue(Qjk[del->delID][d->id])<<" "<<d->id
            <<" "<<k <<endl;
        }
        /*
        env.out()<<"Arc sortants"<<endl;
        for(int j=0;j<_graph.GetArcsOutOfCount(del);j++){
            Arc *a = _graph.GetArcOutOf(del,j);
            if(a->driver== nullptr) continue;
            if(cplex.getValue(x[a->index])<EPS) continue;
            env.out()<<*a<<" a.value "<<cplex.getValue(x[a->index])<<endl;
            env.out()<<Qjk[del->delID][a->driver->id]<<" "<<  cplex.getValue(Qjk[del->delID][a->driver->id])<<endl;
        }
        env.out()<<"Arc entrants"<<endl;

        for(int j=0;j<_graph.GetArcsInOfCount(del);j++){
            Arc *a = _graph.GetArcInOf(del,j);
            if(a->driver== nullptr) continue;
            if(cplex.getValue(x[a->index])<EPS) continue;
            env.out()<<*a<<" a.value "<<cplex.getValue(x[a->index])<<endl;
            env.out()<<Qjk[del->delID][a->driver->id]<<" "<<  cplex.getValue(Qjk[del->delID][a->driver->id])<<endl;
        }
         */
    }
    for(int i=0;i<_graph.GetCustomerCount();i++){
        Customer *c = _graph.GetCustomer(i);
        env.out()<<"G "<<cplex.getValue(g[c->custID])<<endl;
    }
    for(int i=0;i<_graph.GetOrderCount();i++){
        Order *o1 = _graph.GetOrder(i);
        env.out()<<o[o1->orderID]<<  ":"<<cplex.getValue(o[o1->orderID])<<endl;
    }
    for(int k=0;k<_graph.GetDriverCount();++k){
        Driver *d = _graph.GetDriver(k);
        env.out()<<"UnderTime "<< cplex.getValue(UnderTime[d->id])<<" ";
        env.out()<<"Overtime "<< cplex.getValue(Overtime[d->id]);
        env.out()<< " Arrival depot "<<v[d->EndNodeID]<<" "<< cplex.getValue(v[d->EndNodeID])<< " Vs "<< d->start_shift_time+Parameters::NORMAL_WORKING_TIME<< endl;

    }

    for (int i = 0; i < _graph.GetDockCount(); ++i) {
        Dock *dock = _graph.GetDock(i);
//        env.out()<<*dock<<endl;
        for (auto nextId: link_dock_out[dock->dockID]) {
            Dock *next_dock = _graph.GetDock(nextId);
            if(cplex.getValue(Ul[dock->dockID][next_dock->dockID]) <EPS) continue;
//            env.out()<<Ul[dock->dockID][next_dock->dockID]<<" "<<cplex.getValue(Ul[dock->dockID][next_dock->dockID])<<endl;
        }
        for (auto prevId: link_dock_in[dock->dockID]) {
            Dock *prev_dock = _graph.GetDock(prevId);
            if(cplex.getValue(Ul[prev_dock->dockID][dock->dockID]) <EPS) continue;
//            env.out()<<Ul[prev_dock->dockID][dock->dockID]<<" "<<cplex.getValue(Ul[prev_dock->dockID][dock->dockID])<<endl;
        }
    }

}

