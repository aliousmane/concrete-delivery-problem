#ifndef VERTEX
#define VERTEX

#include <stdio.h>

class Vertex
{

public:
    Vertex():id(-1){}
    Vertex(int id):id(id){}
    ~Vertex(){}
    void Show(){
        printf("Vertex %d\n",id);
    }
    int id;
    private:
    
};


#endif // 
