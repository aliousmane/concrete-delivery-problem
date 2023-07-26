#ifndef MAXFLOW
#define MAXFLOW

#include <vector>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Graph.h"

#define MAX_VAL 99999

class MaxFlow
{

public:
    MaxFlow() : visited(0), visitedToken(1), cutList(0), _maxflow(0), _graph(nullptr), graphIsInitialised(false) {}
    MaxFlow(Graph *graph) : visitedToken(1), _graph(graph), cutList(0), _maxflow(0), InitGraph(true)
    {
        VerticesCount = _graph->GetVerticesCount();
        visited.resize(VerticesCount);
        graphIsInitialised = true;
    }

   
    MaxFlow(int n) : visited(0), visitedToken(1), cutList(0), _maxflow(0), InitGraph(false), graphIsInitialised(false)
    {
        VerticesCount = n;
        visited.resize(n);
        _graph = new Graph(n);
    }

    ~MaxFlow()
    {
        if (!InitGraph)
            delete(_graph);
    }
    void Init()
    {
        if (!graphIsInitialised)
        {
            _graph->Init();
            graphIsInitialised = true;
        }
        visited.clear();
        visited.resize(VerticesCount);
        
        _maxflow = 0;
        cutList.clear();
        visitedToken = 1;
    }
    void InitializeGraph(){ _graph->Init();}
    void PrintGraph() { _graph->Show(); }

    void SetVerticesCount(int n) { VerticesCount = n; }

    inline void ShowCut(){

        if(cutList.size() ==0)
            printf("\nNo cut computed yet\n");
        else
        {
            printf("\n");
            for(int cut : cutList)
            {
                printf("%d\t",cut);
            }
             printf("\n");
        }
    }

    /**
     * @brief Solve Maximum flow problem
     * 
     * @param s: source
     * @param t: sink
    */   
    void SolveMaxFlow(int s, int t);

    Graph * GetGraph(){ return _graph;}
    
    std::vector<int> & GetNodesOfMinCut(){ return cutList;}
    
    int GetCutSize() { return (int)cutList.size(); }

    double GetMaxFlow() { return _maxflow; }
    /**
     * @brief Change capacity of en edge
     * 
     * @param from 
     * @param to 
     * @param cap 
     */
    void ChangeEdgeCap(int from, int to, double cap){
        if(graphIsInitialised) _graph->ChangeEdgeCapFast(from,to,cap);
        else _graph->ChangeEdgeCapSlow(from,to,cap);
    }

    void SetSink(int t) { sink = t; }

    /**
     * @brief Add Edge to the graph
     * 
     * @param from 
     * @param to 
     * @param capacity 
     */
    void AddEdge(int from, int to, double capacity);

    /**
     * @brief Remove Edge from the graph and eventually the residual graph
     * 
     * @param from 
     * @param to 
     */
    void RemoveEdge(int from, int to){
        _graph->RemoveEdge(from, to);
    }
   

private:
    double dfs(int node, double flow);
    void visit(int i) { visited[i] = visitedToken; }
    bool isVisited(int i)
    {
        return visited[i] == visitedToken;
    }
    void markAllNodesAsUnvisited()
    {
        visitedToken++;
    }

    Graph *_graph;
    std::vector<int> visited;
    std::vector<int> cutList;
    int visitedToken;
    double _maxflow;
    int VerticesCount; // Nodes size
    int sink;
    bool InitGraph;
    bool graphIsInitialised;
};



#endif // !MAXFLOW
