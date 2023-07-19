#include "cplexSolver.h"

void cplexSolver::createConstraints() {
    C1();
    C2();
    C3();
    C4();
    C5();
    C6();
    C7();
    C8();
    C9();
    C10();
}

void cplexSolver::C1() {
    for (int k = 0; k < _graph.GetDriverCount(); k++) {
        Driver *d = _graph.GetDriver(k);
        Node *start = _graph.GetNode(d->StartNodeID);
        v[start->id].setBounds(start->early_tw, start->late_tw);
        w[start->id].setBounds(start->early_tw, start->late_tw);
        IloExpr expr(env);
        for (int i = 0; i < _graph.GetArcsOutOfCount(start->id); i++) {
            Arc *arc = _graph.GetArcOutOf(start, i);
            expr += x[arc->index];
            model.add(v[arc->to->id] >= w[start->id] - Parameters::MAX_LATE_TW * (1 - x[arc->index]));
        }
        model.add(expr == 1);
        expr.end();
    }
}

void cplexSolver::C2() {
    for (int k = 0; k < _graph.GetDriverCount(); k++) {
        Driver *d = _graph.GetDriver(k);
        Node *end = _graph.GetNode(d->EndNodeID);
        v[end->id].setBounds(end->early_tw, end->late_tw);
        w[end->id].setBounds(end->early_tw, end->late_tw);
        IloExpr expr(env);
        for (int i = 0; i < _graph.GetArcsInOfCount(end->id); i++) {
            Arc *arc = _graph.GetArcInOf(end, i);
            expr += x[arc->index];
            assert(arc->cost == _graph.GetData()->Travel(arc->from, arc->to));
            if (arc->from->type == Parameters::DELIVERY) {
                model.add(v[end->id] >= w[arc->from->id] + arc->cost + Parameters::CLEANING_DURATION -
                                        end->late_tw * (1 - x[arc->index]));
                model.add(v[end->id] <= w[arc->from->id] + arc->cost + Parameters::CLEANING_DURATION +
                                        end->late_tw * (1 - x[arc->index]));
            } else {
                //TODO  add to article
                model.add(v[end->id] >= w[arc->from->id] + arc->cost - end->late_tw * (1 - x[arc->index]));
                model.add(v[end->id] <= w[arc->from->id] + arc->cost + end->late_tw * (1 - x[arc->index]));
            }
        }
        model.add(expr == 1);
        expr.end();
    }
}

void cplexSolver::C3() {

    for (int i = 0; i < _graph.GetDockCount(); ++i) {
        Dock *dock = _graph.GetDock(i);
        Delivery *del = _graph.GetDelivery(dock->delID);
        Depot *dep = _graph.GetData()->GetDepot(dock->depotID);
        for (int j = 0; j < _graph.GetArcsOutOfCount(dock); ++j) {
            Arc *arc = _graph.GetArcOutOf(dock, j);
            //!C5
            model.add(v[arc->to->id] >= w[arc->from->id] + Parameters::ADJUSTMENT_DURATION
                                        + arc->time - _graph.GetData()->LateTW(dock->custID) * (1 - x[arc->index]));
            assert(arc->to->id == del->id);
            assert(arc->driver != nullptr);
            //!C7
            model.add(w[arc->from->id] >= v[arc->from->id] + 60 * Qjk[del->delID][arc->driver->id] / dep->capacity
                                          - dock->late_tw * (1 - x[arc->index]));
        }
    }
}

void cplexSolver::C4() {
    for (int i = 0; i < _graph.GetDeliveryCount(); ++i) {
        Delivery *del = _graph.GetDelivery(i);
        for (int j = 0; j < _graph.GetArcsOutOfCount(del); ++j) {
            Arc *arc = _graph.GetArcOutOf(del, j);
            if (arc->driver == nullptr)
                continue;
            //! C6
            model.add(v[arc->to->id] >= w[arc->from->id] + Parameters::CLEANING_DURATION
                                        + arc->time - _graph.GetData()->LateTW(del->custID) * (1 - x[arc->index]));

        }
        for (int j = 0; j < _graph.GetArcsInOfCount(del); ++j) {
            Arc *arc = _graph.GetArcInOf(del, j);
            if (arc->driver == nullptr)
                continue;
            //! C8
            assert(arc->to->id == del->id);
            model.add(w[arc->to->id] >=
                      v[arc->to->id] + 60 * Qjk[del->delID][arc->driver->id] / Parameters::UNLOADING_RATE
                      - del->late_tw * (1 - x[arc->index]));

            model.add(w[arc->to->id] <=
                      v[arc->to->id] + 60 * Qjk[del->delID][arc->driver->id] / Parameters::UNLOADING_RATE
                      + del->late_tw * (1 - x[arc->index]));
            //! C9
            model.add(w[arc->to->id] <= v[arc->from->id] + Parameters::MAX_TRAVEL_TIME
                                        + arc->to->late_tw * (1 - x[arc->index]));
        }
    }
}

void cplexSolver::C5() {

    for (int i = 0; i < _graph.GetCustomerCount(); i++) {
        Customer *c = _graph.GetCustomer(i);
        IloExpr expr_in_first_orders(env);
        IloExpr expr_out_first_orders(env);
        for (auto o1: _graph.GetData()->GetOrders(c)) {
            Delivery *first = _graph.GetData()->GetDelivery(o1, 0);
            //! C10
            model.add(v[first->id] >= c->early_tw);
            IloExpr expr_in(env);
            int in_count = 0;
            for (int j = 0; j < _graph.GetArcsInOfCount(first); ++j) {
                Arc *arc = _graph.GetArcInOf(first, j);
                if (arc->driver != nullptr)
                    continue;
                auto *from = dynamic_cast<Delivery *>(arc->from);
                expr_in += Ud[from->delID][first->delID];
                expr_in_first_orders += Ud[from->delID][first->delID];
                //! c12
                model.add(v[first->id] >= w[arc->from->id] -
                                          c->late_tw * (1 - Ud[from->delID][first->delID]));
                //! c13
                model.add(v[first->id] <= w[arc->from->id] + _graph.GetData()->GetTimeBtwDel(first, from) +
                                          c->late_tw * (1 - Ud[from->delID][first->delID]));
                ++in_count;
            }
            IloExpr expr_out(env);
            int out_count = 0;
            for (int j = 0; j < _graph.GetArcsOutOfCount(first); ++j) {
                Arc *arc = _graph.GetArcOutOf(first, j);
                if (arc->driver != nullptr)
                    continue;
                auto *to = dynamic_cast<Delivery *>(arc->to);
                expr_out += Ud[first->delID][to->delID];
                expr_out_first_orders += Ud[first->delID][to->delID];
                ++out_count;
            }
            //! c11
            model.add(g[c->custID] >= v[first->id] - c->early_tw - c->late_tw * expr_in);
            //! c17
            if (in_count > 0) {
                model.add(expr_in <= 1);
            }
            //! c16
            if (out_count > 0) {
                model.add(expr_out <= 1);
            }
            //! c18
            if (in_count + out_count > 0) {
                model.add(expr_in + expr_out >= 1);
            }
            expr_in.end();
            expr_out.end();

            for (int j = 0; j < _graph.GetData()->GetDeliveryCount(o1) - 1; ++j) {
                Delivery *prev = _graph.GetData()->GetDelivery(o1, j);
                Delivery *next = _graph.GetData()->GetDelivery(o1, j + 1);
                //! c19
                model.add(v[next->id] >= w[prev->id] -
                                         prev->late_tw * (1 - Ud[prev->delID][next->delID]));
                //! c20
                model.add(v[next->id] <= w[prev->id] + _graph.GetData()->GetTimeBtwDel(prev, next) +
                                         prev->late_tw * (1 - Ud[prev->delID][next->delID]));

                if (j + 2 < _graph.GetData()->GetDeliveryCount(o1)) {
                    Delivery *next_next = _graph.GetData()->GetDelivery(o1, j + 2);
                    //! c21
                    model.add(Ud[prev->delID][next->delID] >= Ud[next->delID][next_next->delID]);
                }
                IloExpr expr_next(env);
                for (int r = 0; r < _graph.GetArcsInOfCount(next); ++r) {
                    Arc *arc = _graph.GetArcInOf(next, r);
                    if (arc->driver == nullptr)
                        continue;
                    expr_next += x[arc->index];
                }
                //! c22
                model.add(Ud[prev->delID][next->delID] >= expr_next);
                expr_next.end();
            }
        }
        if (c->nbOrder > 1) {
            //! c14
            model.add(expr_in_first_orders == c->nbOrder - 1);
            //TODO s'assurer que expr_in_first_orders != expr_out_first_orders
            //! c15
            //if(expr_out_first_orders.getImpl() !=expr_in_first_orders.getImpl())
            {
                model.add(expr_out_first_orders == c->nbOrder - 1);
            }
        }
        expr_in_first_orders.end();
        expr_out_first_orders.end();
    }
}

void cplexSolver::C6() {
    for (int i = 0; i < _graph.GetDockCount(); ++i) {
        Dock *dock = _graph.GetDock(i);
        IloExpr expr_out(env);
        int out_count = 0;
        for (auto nextId: link_dock_out[dock->dockID]) {
            Dock *next_dock = _graph.GetDock(nextId);
            //! c23
            model.add(v[next_dock->id] >= w[dock->id] -
                                          dock->late_tw * (1 - Ul[dock->dockID][next_dock->dockID]));
            expr_out += Ul[dock->dockID][next_dock->dockID];
            ++out_count;
        }
        IloExpr expr_in(env);
        int in_count = 0;
        for (auto prevId: link_dock_in[dock->dockID]) {
            Dock *prev_dock = _graph.GetDock(prevId);
            expr_in += Ul[prev_dock->dockID][dock->dockID];
            ++in_count;
        }
        if (out_count > 0) {
            //! c24
            model.add(expr_out <= 1);
        }
        if (in_count > 0) {
            //! c25
            model.add(expr_in <= 1);
        }
        if (in_count + out_count > 0) {
            //! c26
            IloExpr expr(env);
            int count = 0;
            for (int r = 0; r < _graph.GetArcsInOfCount(dock); ++r) {
                Arc *arc = _graph.GetArcInOf(dock, r);
                expr += x[arc->index];
                ++count;
            }
            if (count > 0) {
                model.add(expr_in + expr_out >= expr);
                if (dock->rank > 0) {
                    //TODO !add to article
                    model.add(expr_in >= expr);
                }
            }
            expr.end();
        }
        {
            Dock *next = _graph.GetData()->GetDock(dock->orderID, dock->rank + 1);
            if (next != nullptr) {
                IloExpr expr_in_next(env);
                int cur_count = 0;
                for (auto prevId: link_dock_in[next->dockID]) {
                    Dock *prev_dock = _graph.GetDock(prevId);
                    expr_in_next += Ul[prev_dock->dockID][next->dockID];
                    cur_count++;
                }
                if (cur_count > 0) {
                    //TODO !add to article
                    model.add(expr_in >= expr_in_next);
                }
                expr_in_next.end();
            }
        }
        expr_out.end();
        expr_in.end();
    }
//    ShowModel();

}

void cplexSolver::C7() {
    //! c27
    for (int i = 0; i < _graph.GetOrderCount(); ++i) {
        Order *o1 = _graph.GetOrder(i);
        IloExpr expr(env);
        for (int j = 0; j < _graph.GetData()->GetDeliveryCount(o1); ++j) {
            Delivery *del = _graph.GetData()->GetDelivery(o1, j);
            for (int k = 0; k < _graph.GetDriverCount(); ++k) {
                Driver *d = _graph.GetDriver(k);
                expr += Qjk[del->delID][d->id];

                IloExpr expr1(env);
                for (int r = 0; r < _graph.GetArcsInOfCount(del); ++r) {
                    Arc *arc = _graph.GetArcInOf(del, r);
                    if (arc->driver == nullptr)continue;
                    if (arc->driver->id != d->id) continue;
                    expr1 += d->capacity * x[arc->index];
                }
                //TODO !Add to article
                model.add(Qjk[del->delID][d->id] <= expr1);
                expr1.end();
            }
        }
        model.add(expr == o1->demand * o[o1->orderID]);
        expr.end();
    }
}

void cplexSolver::C8() {
    //! c28
    for (int i = 0; i < _graph.GetDeliveryCount(); ++i) {
        Delivery *del = _graph.GetDelivery(i);
        IloExpr expr_out(env);
        for (int j = 0; j < _graph.GetArcsOutOfCount(del); ++j) {
            Arc *arc = _graph.GetArcOutOf(del, j);
            if (arc->driver == nullptr) continue;
            expr_out += x[arc->index];
        }
        IloExpr expr_in(env);
        for (int j = 0; j < _graph.GetArcsInOfCount(del); ++j) {
            Arc *arc = _graph.GetArcInOf(del, j);
            if (arc->driver == nullptr)continue;
            expr_in += x[arc->index];
        }
        model.add(expr_out <= 1);
        model.add(expr_out == expr_in);
        expr_out.end();
        expr_in.end();
    }
    for (int i = 0; i < _graph.GetDockCount(); ++i) {
        Dock *dock = _graph.GetDock(i);
        IloExpr expr_out(env);
        for (int j = 0; j < _graph.GetArcsOutOfCount(dock); ++j) {
            Arc *arc = _graph.GetArcOutOf(dock, j);
            if (arc->driver == nullptr)continue;
            expr_out += x[arc->index];
        }
        IloExpr expr_in(env);
        for (int j = 0; j < _graph.GetArcsInOfCount(dock); ++j) {
            Arc *arc = _graph.GetArcInOf(dock, j);
            if (arc->driver == nullptr)continue;
            expr_in += x[arc->index];
        }
        model.add(expr_out <= 1);
        model.add(expr_out == expr_in);
        expr_out.end();
        expr_in.end();
    }
}

void cplexSolver::C9() {
    for (int k = 0; k < _graph.GetDriverCount(); ++k) {
        Driver *d = _graph.GetDriver(k);
        //! c32
        model.add(UnderTime[d->id] >= Parameters::MIN_WORKING_TIME +
                                      d->start_shift_time - v[d->EndNodeID]);
        //! c33
        model.add(Overtime[d->id] >= v[d->EndNodeID] - d->start_shift_time -
                                     Parameters::NORMAL_WORKING_TIME);
        //! c34
        model.add(v[d->EndNodeID] <= d->start_shift_time + Parameters::MAX_WORKING_TIME);
    }
}

void cplexSolver::C10() {
    for (int i = 0; i < _graph.GetNodeCount(); ++i) {
        Node *n = _graph.GetNode(i);
        for (int k = 0; k < _graph.GetDriverCount(); ++k) {
            Driver *d = _graph.GetDriver(k);

            IloExpr expr_out(env);
            int count_out = 0;
            for (int j = 0; j < _graph.GetArcsOutOfCount(n); ++j) {
                Arc *arc = _graph.GetArcOutOf(n, j);
                if (arc->driver == nullptr) continue;
                if (arc->driver->id != d->id) continue;
                expr_out += x[arc->index];
                count_out++;
            }
            IloExpr expr_in(env);
            int count_in = 0;
            for (int j = 0; j < _graph.GetArcsInOfCount(n); ++j) {
                Arc *arc = _graph.GetArcInOf(n, j);
                if (arc->driver == nullptr)continue;
                if (arc->driver->id != d->id) continue;
                expr_in += x[arc->index];
                count_in++;
            }
            if (count_in > 0 and count_out > 0) {
                model.add(expr_out == expr_in);
            }
            expr_out.end();
            expr_in.end();
        }
    }
//    ShowModel();
}