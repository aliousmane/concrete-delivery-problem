#ifndef DFSR
#define DFSR

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Graph.h"

class DFS
{
public:
    DFS(Graph *graph) : _graph(graph), visited(_graph->GetVerticesCount(), false) {}
    ~DFS() {}

    int SolveRecursiveDFS(int at)
    {
        //Vertex already visited
        if (visited[at])
            return 0;

        visited[at] = true;
        int count = 1;

        // Visit all edges adjacent to 'at'
        std::vector<Edge*> edges = _graph->GetEdgesOf(at);
        for (int i = 0; i < edges.size(); i++)
        {
            Edge * e = edges[i];
            count += SolveRecursiveDFS(e->to);
        }

        return count;
    }

private:
    Graph *_graph;
    std::vector<bool> visited;
};

#endif