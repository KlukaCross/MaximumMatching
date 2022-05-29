#ifndef MAXIMUMMATCHING_NODE_H
#define MAXIMUMMATCHING_NODE_H
#include "QPoint"

class Node {
public:
    Node(QPointF coordinates);
    QPointF get_coordinates();
    void set_coordinates(QPointF coordinates);
private:
    QPointF coors;
};

bool operator==(Node node1, Node node2);

#endif //MAXIMUMMATCHING_NODE_H
