
#include "Graph.h"

void Graph::Init() {
    _arcs.clear();
    _arcs_in_of.resize(_data->GetNodeCount());
    _arcs_out_of.resize(_data->GetNodeCount());
    for (int k = 0; k < _data->GetDriverCount(); k++) {
        Driver *d = _data->GetDriver(k);
        Node *start = _data->GetNode(d->StartNodeID);
        Node *end = _data->GetNode(d->EndNodeID);
        AddArc({start, end, d, _arcs.size()});

        for (int i = 0; i < _data->GetDeliveryCount(); i++) {
            Delivery *del = _data->GetDelivery(i);
            Dock *dock = _data->GetDock(del->dockID);
            AddArc({start, dock, d, _arcs.size()});
            AddArc({dock, del, d, _arcs.size()});
            AddArc({del, end, d, _arcs.size()});
            Order *o1 = _data->GetOrder(del->orderID);
            int nbDelMin = std::ceil(double(o1->demand) / _data->maxDriverCap);
            for (int j = 0; j < _data->GetDeliveryCount(); j++) {
                if (i == j) continue;
                Delivery *next_del = _data->GetDelivery(j);
                if (_data->LateTW(next_del) < _data->EarlyTW(del)) {
                    continue;
                }
                if (next_del->orderID == del->orderID) {
                    if (next_del->rank < del->rank) {
                        continue;
                    }
                    if (next_del->rank == del->rank + 1) {
                        AddArc({del, next_del, nullptr, _arcs.size()});
                    }
                } else if (next_del->custID == del->custID) {
                    if (del->rank >= nbDelMin-1) {
                        if (next_del->rank == 0) {
                            AddArc({del, next_del, nullptr, _arcs.size()});
                        }
                    }
                }
                Dock *next_dock = _data->GetDock(next_del);
                AddArc({del, next_dock, d, _arcs.size()});
            }
        }
    }
    printf("Arc size %d\n", (int)_arcs.size());


    for(int i=0;i < _arcs.size();i++){
        Arc * p_a = &_arcs[i] ;

        p_a->cost = _data->Travel(p_a->from, p_a->to);
        p_a->time = _data->Travel(p_a->from, p_a->to);

        _arcs_out_of[p_a->from->id].push_back( p_a);
        _arcs_in_of[p_a->to->id].push_back(p_a);
        int k = (p_a->driver == nullptr) ? _data->GetDriverCount() + 1 : p_a->driver->id + 1;
        ArcKey key{p_a->from->id, p_a->to->id, k};

        _map_arcs[key.get_hash()] = p_a;

        ArcKey key1{p_a->to->id, p_a->from->id, k};
        _map_arcs[key1.get_hash()] = p_a;
    }

    pos_arcs_in_of.resize(_data->GetNodeCount());
    pos_arcs_out_of.resize(_data->GetNodeCount());
    for (int i = 0; i < _data->GetNodeCount(); i++) {
        pos_arcs_in_of[i].resize(GetArcsInOfCount(i));
        pos_arcs_out_of[i].resize(GetArcsOutOfCount(i));
    }
}

void Graph::AddArc(Arc const  &a){
    if(ArcMap.contains(a.get_hash()))
        return;
    ArcMap[a.get_hash()]=a;
    _arcs.push_back(a);

}
Arc Graph::CreateArc(Node *from, Node *to, Driver *d, size_t index) {
    Arc ar;
    ar.from = from;
    ar.to = to;
    ar.driver = d;
    ar.index = (int)index;
    ar.cost = _data->Travel(from, to);
    ar.time = _data->Travel(from, to);
    return ar;
}

Arc Graph::CreateArc(Node *from, Node *to, Driver *d) {
    Arc ar;
    ar.from = from;
    ar.to = to;
    ar.driver = d;
    ar.index = (int)_arcs.size();
    ar.cost = _data->Travel(from, to);
    ar.time = _data->Travel(from, to);
    return ar;
}
