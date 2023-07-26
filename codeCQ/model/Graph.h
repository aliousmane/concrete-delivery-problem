

#ifndef CODE_GRAPH_H
#define CODE_GRAPH_H

#include "Arc.h"
#include <map>
#include <vector>
#include <unordered_map>
#include "../Data.h"
class GraphHashFunction {
public:
    size_t operator()(const std::string &s) const {
        return std::hash<std::string>()(s) << 1;
    };
};
class Graph {
public:
    explicit Graph(Data *data):_data(data),_arcs(0),
    _arcs_in_of(0),_arcs_out_of(0),pos_arcs_in_of(0),pos_arcs_out_of(0) {
        Init();
    }
    int GetArcsInOfCount(int index) { return (int)_arcs_in_of[index].size(); }
    int GetArcsInOfCount(Node *n) { return (int)_arcs_in_of[n->id].size(); }
    int GetArcsOutOfCount(int index) { return (int)_arcs_out_of[index].size(); }
    int GetArcsOutOfCount(Node *n) { return (int)_arcs_out_of[n->id].size(); }
    Arc * GetArcOutOf(int node,int index ) { return _arcs_out_of[node][index]; }
    Arc * GetArcOutOf(Node * n,int index ) { return _arcs_out_of[n->id][index]; }
    Arc * GetArcInOf(int node,int index ) { return _arcs_in_of[node][index]; }
    Arc * GetArcInOf(Node * n,int index ) { return _arcs_in_of[n->id][index]; }
    int GetArcCount(){return  (int)_arcs.size();}
    int GetCustomerCount() const{return _data->GetCustomerCount();}
    int GetOrderCount() const{return _data->GetOrderCount();}
    int GetNodeCount() const{return _data->GetNodeCount();}
    int GetDockCount() const{return _data->GetDockCount();}
    int GetDeliveryCount() const{return _data->GetDeliveryCount();}
    int GetDriverCount() const{return _data->GetDriverCount();}
    Driver * GetDriver(int k){return  _data->GetDriver(k);}
    Node * GetNode(int id){return  _data->GetNode(id);}
    Dock * GetDock(int id){return  _data->GetDock(id);}
    Delivery * GetDelivery(int id){return  _data->GetDelivery(id);}
    Customer * GetCustomer(int id){return  _data->GetCustomer(id);}
    Order * GetOrder(int id){return  _data->GetOrder(id);}
    Data * GetData(){return _data;}
    Arc * GetArc(int i){ return &_arcs[i];}


private:
    Data *_data;
    std::vector< Arc > _arcs;
    std::vector<std::vector< Arc* >> _arcs_in_of;
    std::vector<std::vector< Arc* >> pos_arcs_in_of;
    std::vector<std::vector< Arc* >> _arcs_out_of;
    std::vector<std::vector< Arc* >> pos_arcs_out_of;
    std::map<u_int, Arc*> _map_arcs;
    std::unordered_map<size_t , Arc> ArcMap;

    void Init();
    Arc CreateArc(Node *from, Node *to, Driver *d, size_t index) ;
    Arc CreateArc(Node *from, Node *to, Driver *d) ;
    void AddArc(Arc const &a);
};


#endif //CODE_GRAPH_H
