#ifndef MAXIMUMMATCHING_EDGE_H
#define MAXIMUMMATCHING_EDGE_H
#include "node.h"

class Edge {
public:
    Edge(Node* from, Node* to = nullptr);
    Node* get_from();
    Node* get_to();
    void set_from(Node* node);
    void set_to(Node* node);
private:
    Node* from;
    Node* to;
};

#endif //MAXIMUMMATCHING_EDGE_H
