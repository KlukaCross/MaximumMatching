#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) {
    auto *actionGroup = new QActionGroup(this);
    moveAction = new QAction(tr("&Move"), this);
    moveAction->setCheckable(true);
    createAction = new QAction(tr("&Create"), this);
    createAction->setCheckable(true);
    connectAction = new QAction(tr("&Connect"), this);
    connectAction->setCheckable(true);
    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setCheckable(true);
    searchAction = new QAction(tr("&Search"), this);
    connect(searchAction, SIGNAL(triggered(bool)), this, SLOT(search_max_matching()));
    actionGroup->addAction(moveAction);
    actionGroup->addAction(createAction);
    actionGroup->addAction(connectAction);
    actionGroup->addAction(deleteAction);
    actionGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    cur_node = nullptr;
    cur_edge = nullptr;
    this->ToolBar = addToolBar("Tools");
    ToolBar->addAction(moveAction);
    ToolBar->addAction(createAction);
    ToolBar->addAction(connectAction);
    ToolBar->addAction(deleteAction);
    ToolBar->addAction(searchAction);

    fieldWidget = new FieldDrawing(NODE_RADIUS, &nodes, &edges, &matching, &cur_edge, this);
    setCentralWidget(fieldWidget);
    fieldWidget->installEventFilter(this);

    QTimer::singleShot(1000/this->FPS, this, SLOT(update_all()));
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj != fieldWidget) return QMainWindow::eventFilter(obj, event);
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseMove) {
        auto* mouseEvent = dynamic_cast<QMouseEvent *>(event);
        if (mouseEvent->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
            matching.clear();
            Node *sel_node = get_select_node(mouseEvent->pos());
            if (moveAction->isChecked()) {
                if (sel_node == nullptr) return true;
                cur_node = sel_node;
            } else if (createAction->isChecked()) {
                if (sel_node != nullptr) return true;
                Node *new_node = new Node(mouseEvent->pos());
                nodes.push_back(new_node);
            } else if (connectAction->isChecked()) {
                if (sel_node == nullptr) return true;
                if (cur_edge == nullptr) {
                    Edge *new_edge = new Edge(sel_node);
                    cur_edge = new_edge;
                } else {
                    cur_edge->set_to(sel_node);
                    edges.push_back(cur_edge);
                    cur_edge = nullptr;
                }
            } else if (deleteAction->isChecked()) {
                if (sel_node == nullptr) return true;
                delete_edges_without_node(sel_node);
                nodes.removeOne(sel_node);
                delete sel_node;
            }
        } else if (mouseEvent->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::RightButton) {
            matching.clear();
            break_tools();
        } else if (mouseEvent->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton) {
            matching.clear();
            if (moveAction->isChecked())
                cur_node = nullptr;
        } else if (mouseEvent->type() == QEvent::MouseMove) {
            if (moveAction->isChecked()) {
                if (cur_node == nullptr) return true;
                cur_node->set_coordinates(mouseEvent->pos());
            }
        }
        return true;
    }
    if (event->type() == QEvent::Leave) {
        if (connectAction->isChecked()) {
            if (cur_edge == nullptr) return true;
            delete cur_edge;
            cur_edge = nullptr;
        } else if (moveAction->isChecked())
            cur_node = nullptr;
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::break_tools() {
    if (moveAction->isChecked()) {
        moveAction->setChecked(false);
        cur_node = nullptr;
    } else if (createAction->isChecked())
        createAction->setChecked(false);
    else if (connectAction->isChecked()) {
        connectAction->setChecked(false);
        delete cur_edge;
        cur_edge = nullptr;
    } else if (deleteAction->isChecked())
        deleteAction->setChecked(false);
}

Node* MainWindow::get_select_node(QPoint coordinates) {
    for (auto v: nodes) {
        if (pow(coordinates.x()-v->get_coordinates().x(), 2) +
        pow(coordinates.y()-v->get_coordinates().y(), 2) <= pow(NODE_RADIUS, 2)) {
            return v;
        }
    }
    return nullptr;
}

void MainWindow::delete_edges_without_node(Node *delete_node) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < edges.size(); ++i) {
            Edge* ed = edges[i];
            if (ed->get_to() == delete_node || ed->get_from() == delete_node) {
                ed->set_to(nullptr);
                ed->set_from(nullptr);
                edges.remove(i);
                delete ed;
                changed = true;
                break;
            }
        }
    }
}

void MainWindow::update_all() {
    fieldWidget->update();
    QTimer::singleShot(1000/this->FPS, this, SLOT(update_all()));
}

void MainWindow::search_max_matching() {
    break_tools();
    matching.clear();
    QHash<Edge*, Edge*> comparison_edges;
    QHash<Node*, QVector<Edge*>> transport_network;
    QSet<Node*> A, B;
    for (auto *v: nodes) {
        transport_network[v] = QVector<Edge*>();
    }
    for (auto &e: edges) {
        Edge* new_e = new Edge(e->get_from(), e->get_to());
        transport_network[e->get_from()].push_back(new_e);
        comparison_edges[new_e] = e;
    }
    if (nodes.empty()) {
        QMessageBox msgBox;
        msgBox.setText("Graph is empty!");
        msgBox.exec();
        return;
    }
    if (!splitting_simple(transport_network, A, B)) {
        QMessageBox msgBox;
        msgBox.setText("Graph is not simple!");
        msgBox.exec();
        return;
    }

    ford_fulkerson(transport_network, A, B);
    for (auto &b: B) {
        if (!transport_network[b].isEmpty())
            matching.push_back(comparison_edges[transport_network[b][0]]);
    }

    for (auto it = comparison_edges.cbegin(), end = comparison_edges.cend(); it != end; ++it)
        delete it.key();
}

bool MainWindow::splitting_simple(QHash<Node*,QVector<Edge*>> &graph, QSet<Node*> &A, QSet<Node*> &B) {
    for (QVector<Edge *> &edg: graph) {
        for (auto &e: edg) {
            if (!graph[e->get_to()].isEmpty()) return false;
            A.insert(e->get_from());
            B.insert(e->get_to());
        }
    }
    return true;
}

bool MainWindow::dfs(QHash<Node*,QVector<Edge*>> &graph, Node* u, QHash<Node*, bool> &visited, QHash<Node*, bool> &missing) {
    if (visited[u])
        return false;
    visited[u] = true;
    for (auto e: graph[u]) {
        if (!missing[e->get_to()] || dfs(graph, e->get_to(), visited, missing)) {
            missing[e->get_to()] = true;
            missing[u] = true;
            Node* tmp = e->get_to();
            e->set_to(e->get_from());
            e->set_from(tmp);
            graph[u].removeOne(e);
            graph[e->get_from()].push_back(e);
            return true;
        }
    }
    return false;
}


void MainWindow::ford_fulkerson(QHash<Node*,QVector<Edge*>> &graph, QSet<Node*> &A, QSet<Node*> &B) {
    bool isPath = true;
    QHash<Node*, bool> missing;
    for (auto it = graph.cbegin(); it != graph.cend(); it++)
        missing[it.key()] = false;
    while (isPath) {
        isPath = false;
        QHash<Node*, bool> visited;
        for (auto it = graph.cbegin(); it != graph.cend(); it++)
            visited[it.key()] = false;
        for (auto &a: A) {
            if (!missing[a] && dfs(graph, a, visited, missing))
                isPath = true;
        }
    }
}