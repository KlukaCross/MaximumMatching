#include "field_drawing.h"

FieldDrawing::FieldDrawing(int node_radius, QVector<Node*> *nodes, QVector<Edge*> *edges,
                           QVector<Edge*> *matching, Edge** cur_edge, QWidget *parent) : QWidget(parent) {
    this->node_radius = node_radius;
    this->nodes = nodes;
    this->edges = edges;
    this->matching = matching;
    this->cur_edge = cur_edge;
}

void FieldDrawing::draw_arrow(QPainter &painter, QPointF start, QPointF end) {
    double dx = start.x() - end.x();
    double dy = start.y() - end.y();
    double angle_from = atan2(dx, dy);
    QPointF p0(start.x(),start.y());
    QPointF p1(end.x(),end.y());
    double len_line = sqrtf(pow(p1.x()-p0.x(),2) + pow(p1.y()-p0.y(), 2));
    painter.drawLine(p0, p1);
    QPolygonF arrow;
    QPointF p2(p0.x()-sin(angle_from)*len_line*ARROW_POS, p0.y()-cos(angle_from)*len_line*ARROW_POS);
    QPointF p3(p2.x() + ARROW_HEIGHT*sin(angle_from) - ARROW_WIDTH/2*cos(angle_from),
               p2.y() + ARROW_HEIGHT*cos(angle_from) + ARROW_WIDTH/2*sin(angle_from));
    QPointF p4(p2.x() + ARROW_HEIGHT*sin(angle_from) + ARROW_WIDTH/2*cos(angle_from),
               p2.y() + ARROW_HEIGHT*cos(angle_from) - ARROW_WIDTH/2*sin(angle_from));
    arrow << p2 << p3 << p4;
    painter.drawConvexPolygon(arrow);
}

void FieldDrawing::paintEvent(QPaintEvent *ev) {
    QPainter painter(this);
    painter.setPen(QPen(this->EDGE_COLOR, LINE_WIDTH, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(this->EDGE_COLOR, Qt::SolidPattern));
    for (auto e: *edges) {
        QPointF p0(e->get_from()->get_coordinates().x(),e->get_from()->get_coordinates().y());
        QPointF p1(e->get_to()->get_coordinates().x(),e->get_to()->get_coordinates().y());
        draw_arrow(painter, p0, p1);
    }
    if (*cur_edge != nullptr) {
        QPointF pos_to = this->mapFromGlobal(QCursor::pos());
        QPointF p0((*cur_edge)->get_from()->get_coordinates().x(), (*cur_edge)->get_from()->get_coordinates().y());
        draw_arrow(painter, p0, pos_to);
    }
    painter.setPen(QPen(this->MATCHING_COLOR, LINE_WIDTH, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(this->MATCHING_COLOR, Qt::SolidPattern));
    for (auto e: *matching) {
        QPointF p0(e->get_from()->get_coordinates().x(),e->get_from()->get_coordinates().y());
        QPointF p1(e->get_to()->get_coordinates().x(),e->get_to()->get_coordinates().y());
        draw_arrow(painter, p0, p1);
    }

    painter.setPen(QPen(this->NODE_COLOR, LINE_WIDTH, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(this->NODE_COLOR, Qt::SolidPattern));
    for (auto n: *nodes) {
        painter.drawEllipse(n->get_coordinates(), node_radius, node_radius);
    }

}