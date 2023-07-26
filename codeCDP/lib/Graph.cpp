#include "Graph.h"

bool operator==(Edge const &a, Edge const &b)
{
    return a.isEqual(b);
}

void Graph::AddEdge(int de, int vers, double capacity)
{
    Edge e1(de, vers, capacity);
    Edge e2(vers, de, 0);
    e2.isResidual = true;

    AddEdge(e1);
    AddEdge(e2);
}
void Graph::AddEdge(Edge &e)
{
    std::pair<int, int> p1(e.from, e.to);
    std::pair<std::pair<int, int>, Edge> p2;

    e.index = ListEdges.size();
    if (!e.isResidual){
        ListEdges[p1] = e;
    }
    else
    {
        Edge e1 = ListEdges[p1];
        if (e1 == e)
            return;
        ListEdges[p1] = e;
    }
   
}

void Graph::Init()
{
    //printf("Init Graph\n");
    for (size_t i = 0; i < ListVertices.size(); i++)
        Edges_Of[i].clear();

    for (auto &m : ListEdges)
    {
        Edge *e = &m.second;
        e->Init();
        Edges_Of[e->from].push_back(e);
    }

    CreateResidual();
}

void Graph::CreateResidual()
{
    for (auto &m : ListEdges)
    {
        Edge *e = &m.second;

        if (e->walked_on)
        {
            continue;
        }
        //printf("walked_on %d\n",(int)e->walked_on);
        //e->Show();
        for (size_t j = 0; j < GetEdgesOfCount(e->to); j++)
        {
            Edge *e2 = GetEdgeOf(e->to, j);
            if (e2->to == e->from)
            {
                // e2->Show();
                e->residual = e2;
                e2->residual = e;
                e2->walked_on = true;
                //printf("walked_on e2 %d\n",(int)e2->walked_on);

                break;
            }
        }
        e->walked_on = true;
        //e->Show();
    }
}

void Graph::ChangeEdgeCapSlow(int from, int to, double cap)
{

    Edge *e = GetEdge(from, to);
    e->capacity = cap;
}
void Graph::ChangeEdgeCapFast(int from, int to, double cap)
{
    Edge *e = GetEdge(from, to);
    e->capacity = cap;
    /*
        for (int j = 0; j < GetEdgesOfCount(from); j++)
        {
            Edge * e = GetEdgeOf(from,j);   
            if(e->to == to){
                e->capacity = cap;
                break;
            }
        }
    */
}

void Graph::AddVertex(int index) {}
void Graph::AddVertex(int index, Vertex &v)
{
    ListVertices[index] = v;
}

void Graph::RemoveEdge(int from, int to)
{
    for (int j = 0; j < GetEdgesOfCount(from); j++)
    {
        Edge *e = GetEdgeOf(from, j);
        if (e->to == to)
        {
            break;
        }
    }
}