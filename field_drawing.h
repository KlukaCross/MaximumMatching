#ifndef MAXIMUMMATCHING_FIELD_DRAWING_H
#define MAXIMUMMATCHING_FIELD_DRAWING_H
#include "node.h"
#include "edge.h"
#include <QWidget>
#include <QPainter>
#include <QVector>

class FieldDrawing : public QWidget {
public:
    FieldDrawing(int node_radius, QVector<Node*> *nodes, QVector<Edge*> *edges,
                 QVector<Edge*> *matching, Edge** cur_edge, QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *e) override;

private:
    const double ARROW_WIDTH = 20;
    const double ARROW_HEIGHT = 10;
    const double ARROW_POS = 0.7;  // position of end of the arrow; 0 - start of line, 1 - end of line

    const int LINE_WIDTH = 2;
    const QColor NODE_COLOR = Qt::black;
    const QColor EDGE_COLOR = Qt::gray;
    const QColor MATCHING_COLOR = Qt::green;

    int node_radius;
    QVector<Node*> *nodes;
    QVector<Edge*> *edges;
    QVector<Edge*> *matching;
    Edge** cur_edge;

    void draw_arrow(QPainter &painter, QPointF start, QPointF end);
};


#endif //MAXIMUMMATCHING_FIELD_DRAWING_H
