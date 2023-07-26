

#ifndef CODECQ_ARC_H
#define CODECQ_ARC_H

#include "../Node.h"
#include "../Driver.h"

class Arc {
public:

    Arc():index(-1) {}
    Arc(Node *de,Node *vers,Driver *d, int id){
        from=de;
        to=vers;
        driver=d;
        index=id;
    }
    Arc(Node *de,Node *vers,Driver *d, size_t id){
        from=de;
        to=vers;
        driver=d;
        index=int(id);
    }
    Node *from{nullptr};
    Node *to{nullptr};
    Driver *driver{nullptr};
    int index;
    double cost{0};
    double time{0};
    double value{0};
    bool walked_on{false};
    friend std::ostream &operator<<(std::ostream &os, const Arc &a){
        if(a.driver== nullptr){
            os<<a.from->c<<""<<a.from->id<<"->"<< a.to->c<<""<< a.to->id;
        }
        else{
            os<<a.from->c<<""<<a.from->id<<"->"<< a.to->c<<""<< a.to->id<<"-"<<a.driver->id;
        }
        return os;
    }
    void Show() const {
        if (driver != nullptr) printf("x%d_%d_%d:%.2f\n", from->id, to->id, driver->id, value);
        else printf("x%d_%d_(-1):%.2f\n", from->id, to->id, value);
    }
    std::size_t get_hash() const {
        int d_id = (driver== nullptr) ? -1 : driver->id;
        return cantor(d_id, cantor(from->id, to->id));
    }
    static int cantor(int a, int b) {
        return (a + b + 1) * (a + b) / 2 + b;
    }

};

struct ArcKey {
    int from;
    int to;
    int d_id;

    std::size_t get_hash() const {
        return cantor(d_id, cantor(from, to));
    }

    static int cantor(int a, int b) {
        return (a + b + 1) * (a + b) / 2 + b;
    }

};

#endif //CODECQ_ARC_H
