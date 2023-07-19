#ifndef GRAPH
#define GRAPH

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <utility>
#include "Vertex.h"


class Edge
{
public:
    Edge():index(-1), cost(0),capacity(0), flow(0), residualFlow(0),walked_on(false), residual(nullptr), isResidual(false), from(0),to(0)
    {
    }
    Edge(int de, int vers, double cap):from(de), to(vers), cost(0),capacity(cap), flow(0), residualFlow(0), walked_on(false), residual(nullptr),isResidual(false){
    }

    ~Edge(){ }

    void Init(){ flow=0; walked_on = false; residual = nullptr; }

    //Edge(Vertex * de, Vertex * vers, double cost ):from(de), to(vers), cost(cost){}
    double remainingCapacity(){return capacity - flow; }
    void augment(double bottleNeck) {
        flow += bottleNeck;
        residual->flow -= bottleNeck;
    }

    void Show(){

        if(residual != nullptr)
           // printf("E%d_%d_%d_R%d_%d\n",from,to,capacity,residual->from, residual->to);
            printf("mxf.AddEdge(%d,%d,%lf);\n",from,to,capacity);
        else
            printf("E%d_%d_%lf\n",from,to,capacity);
    }

    bool isEqual(Edge const& b) const
    {
        return (from == b.from && to == b.to);
    }
    //bool Edge::operator==(Edge const& a, Edge const& b)
    
    Edge * residual;
    bool isResidual;
    int residualFlow;
    int from;//Vertex *from;
    int to; //Vertex *to;
    double cost;
    double capacity;
    double flow;
    int index;
    bool walked_on;
    private:
        
};


class Graph
{
public:
    Graph(int n):ListEdges(){
        ListVertices.resize(n);
        Edges_Of.resize(n);
        for (size_t i = 0; i < n; i++)
            Edges_Of[i].resize(0);  
    }
   
    std::map< std::pair<int,int>, Edge> &  GetAllEdges(){
        return ListEdges;
    }
    Edge * GetEdge(int from, int to){
        std::pair<int,int> p(from,to);
        return &ListEdges[p];}
        
    inline std::vector<Vertex> &  GetAllVertices(){
        return ListVertices;
    }
    inline Vertex GetVertex(int i){return ListVertices[i];}

    inline int GetEdgesCount(){return (int)ListEdges.size();}

    inline int GetVerticesCount(){return (int)ListVertices.size();}
    inline void SetVerticesCount(int count){ListVertices.resize(count);}

    inline std::vector<Edge*> &  GetEdgesOf(int i){
        return Edges_Of[i];
    }
    inline Edge *  GetEdgeOf(int i,int j){
        return Edges_Of[i][j];
    }
    inline int GetEdgesOfCount(int i){return (int)Edges_Of[i].size();}
    inline int GetEdgesOfCount(){return (int)Edges_Of.size();}
     /**
     * @brief Change capacity of en edge
     * 
     * @param from 
     * @param to 
     * @param cap 
     */
    void ChangeEdgeCapSlow(int from, int to, double cap);
    void ChangeEdgeCapFast(int from, int to, double cap);
    void Init();
    void AddEdge(int from, int to, double capacity);
    void AddEdge(Edge &e);
    void AddVertex(int index);
    void AddVertex(int index,Vertex & v);
    void CreateResidual();
    void RemoveEdge(int from, int to);
    void Show(){
        printf("%d Vertices:  Edges %d\n",(int)ListVertices.size(), (int)ListEdges.size());
    }
    void ShowEdges(){

        for (auto& m  : ListEdges)
        { 
           m.second.Show();
        }
        
    }
    private:
    std::map<std::pair<int,int>,Edge> ListEdges;
    std::vector<Vertex> ListVertices;
    std::vector<std::vector<Edge*>> Edges_Of;

};



#endif // !GRAPH
