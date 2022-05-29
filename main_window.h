#ifndef MAXIMUMMATCHING_MAIN_WINDOW_H
#define MAXIMUMMATCHING_MAIN_WINDOW_H
#include "node.h"
#include "edge.h"
#include "field_drawing.h"
#include <QMainWindow>
#include <QVector>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QActionGroup>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QTimer>
#include <QQueue>
#include <QMessageBox>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);
public slots:
    void update_all();
    void search_max_matching();

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    const int NODE_RADIUS = 20;
    const int FPS = 60;

    QAction *moveAction;
    QAction *createAction;
    QAction *connectAction;
    QAction *deleteAction;
    QAction *searchAction;

    QToolBar* ToolBar;

    FieldDrawing *fieldWidget;

    Node* cur_node;
    Edge* cur_edge;
    QVector<Node*> nodes;
    QVector<Edge*> edges;
    QVector<Edge*> matching;

    void break_tools();
    Node* get_select_node(QPoint coordinates);
    void delete_edges_without_node(Node *delete_node);
    bool check_simple(QHash<Node*,QVector<Edge*>> &graph);
    bool dfs(QHash<Node*,QVector<Edge*>> &graph, Node* source, Node* stock,
             QVector<Edge*> &path, QVector<Node*> &visited, QHash<Edge*, int> &flow);
    Edge* get_edge(QHash<Node*,QVector<Edge*>> &graph, Node* from, Node* to);
    void ford_fulkerson(QHash<Node*,QVector<Edge*>> &graph, QHash<Edge*, int> &flow, Node* source, Node* stock);
};


#endif //MAXIMUMMATCHING_MAIN_WINDOW_H
