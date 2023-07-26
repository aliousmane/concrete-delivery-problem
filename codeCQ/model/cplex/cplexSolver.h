

#ifndef CODE_CPLEXMODEL_H
#define CODE_CPLEXMODEL_H

#include <ilcplex/ilocplex.h>
#include <set>
#include <vector>
#include "../Graph.h"

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVar3Matrix;

class cplexSolver {
public:
    explicit cplexSolver(Graph &graph) : _graph(graph), env(), model(env), link_dock_in(graph.GetDockCount()),
                                         link_dock_out(graph.GetDockCount()) {
        end_time = std::numeric_limits<time_t>::max();

    }

    void run();

    time_t end_time;

private:
    Graph &_graph;
    IloEnv env;
    IloModel model;
    IloObjective obj_func;
    IloCplex cplex;

    IloNumVarArray x;
    IloNumVarArray v;
    IloNumVarArray w;
    IloNumVarArray y;
    IloNumVarArray o;
    IloNumVarArray g;
    IloNumVarArray UnderTime;
    IloNumVarArray Overtime;
    NumVar3Matrix u; // Link between 2 orders
    NumVarMatrix Ud; // Link between 2 delivery nodes
    NumVarMatrix Ul; // Link between 2 loading nodes
    NumVarMatrix Qjk; // load of del j with driver k

    std::vector<std::set<int>> link_dock_out, link_dock_in;

    void ShowModel() {
        env.out() << model << "\n";
        exit(1);
    }

    void createVariables();

    void createObjective();

    void createConstraints();

    void setSettings();

    void solveModel();

    void C1();

    void C2();

    void C3();

    void C4();

    void C5();

    void C6();

    void C7();

    void C8();

    void C9();

    void C10();

};


#endif //CODE_CPLEXMODEL_H
