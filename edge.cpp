#include "edge.h"

Node* Edge::get_from() {
    return this->from;
}

Node* Edge::get_to() {
    return this->to;
}

void Edge::set_from(Node* node) {
    this->from = &(*node);
}

void Edge::set_to(Node* node) {
    this->to = &(*node);
}

Edge::Edge(Node *from, Node* to) {
    this->from = &(*from);
    this->to = to;
}
