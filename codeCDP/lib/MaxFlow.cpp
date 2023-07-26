#include "MaxFlow.h"

void MaxFlow::SolveMaxFlow(int s, int t){
    Init();

    for(double f = dfs(s, MAX_VAL); f!=(double)0; f= dfs(s, MAX_VAL)){
        //printf("Flow %lf \n",f);
        markAllNodesAsUnvisited();
        _maxflow +=f;
    }

    //Find min cut
    cutList.clear();
    for (size_t i = 0; i < VerticesCount; i++)
        if(isVisited(i))
            cutList.push_back(i);

}

double MaxFlow::dfs(int node, double flow){
    //printf("dsf from node %d\n",node);
    // At sink node, return augmented path flow.
    
    if (node == sink) return flow; 

    std::vector<Edge*> edges = _graph->GetEdgesOf(node);
    visit(node);

    for(Edge * edge : edges){
        //printf("Node visite %d: flow = %lf\n",edge->to,flow);
        double rcap = edge->remainingCapacity();
        //printf("rcap %lf\n",rcap);
        if (rcap > 0 && !isVisited(edge->to)){
           
            double bottleNeck = dfs(edge->to, std::min(flow,rcap));
            
            // Augment flow with bottle neck value
            if (bottleNeck > 0) {
                edge->augment(bottleNeck);
                return bottleNeck;
            }
        }
    }
    return 0;
}

void  MaxFlow::AddEdge(int from, int to, double capacity){
    _graph->AddEdge( from,  to,  capacity);
}