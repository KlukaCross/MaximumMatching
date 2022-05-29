#include "node.h"

Node::Node(QPointF coordinates) {
    this->coors = coordinates;
}

QPointF Node::get_coordinates() {
    return this->coors;
}

void Node::set_coordinates(QPointF coordinates) {
    this->coors = coordinates;
}

bool operator==(Node node1, Node node2) {
    return node1.get_coordinates() == node2.get_coordinates();
}