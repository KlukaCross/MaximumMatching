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
    QHash<Node*,QVector<Edge*>> graph;
    for (auto v: nodes) {
        graph[v] = QVector<Edge*>();
    }
    for (auto e: edges) {
        graph[e->get_from()].push_back(e);
    }
    if (nodes.empty()) {
        QMessageBox msgBox;
        msgBox.setText("Graph is empty!");
        msgBox.exec();
        return;
    }
    if (!check_simple(graph)) {
        QMessageBox msgBox;
        msgBox.setText("Graph is not simple!");
        msgBox.exec();
        return;
    }

    QHash<Edge*, Edge*> comparison_edges;
    QHash<Node*, QVector<Edge*>> transport_network;
    QHash<Edge*, int> flow;
    Node *source = new Node(QPointF(0,0));
    Node *stock = new Node(QPointF(0,0));
    transport_network[source] = QVector<Edge*>();
    transport_network[stock] = QVector<Edge*>();
    for (auto v: nodes) {
        transport_network[v] = QVector<Edge*>();
    }
    for (auto e: edges) {

        Edge *rev_e = new Edge(e->get_to(), e->get_from());
        Edge *rev_rev_e = new Edge(e->get_from(), e->get_to());
        comparison_edges[rev_e] = e;
        if (transport_network.find(e->get_from())->isEmpty()) {
            Edge *to_stock = new Edge(e->get_from(), stock);
            Edge *from_stock = new Edge(stock, e->get_from());
            transport_network[e->get_from()].push_back(to_stock);
            transport_network[e->get_from()].push_back(from_stock);
            transport_network[stock].push_back(from_stock);
            transport_network[stock].push_back(to_stock);
            flow[to_stock] = 0;
            flow[from_stock] = 0;
        }
        if (transport_network.find(e->get_to())->isEmpty()) {
            Edge *to_source = new Edge(e->get_to(), source);
            Edge *from_source = new Edge(source, e->get_to());
            transport_network[e->get_to()].push_back(to_source);
            transport_network[e->get_to()].push_back(from_source);
            transport_network[source].push_back(from_source);
            transport_network[source].push_back(to_source);
            flow[to_source] = 0;
            flow[from_source] = 0;
        }
        transport_network[e->get_to()].push_back(rev_e);
        transport_network[e->get_to()].push_back(rev_rev_e);
        transport_network[e->get_from()].push_back(rev_e);
        transport_network[e->get_from()].push_back(rev_rev_e);
        flow[rev_e] = 0;
        flow[rev_rev_e] = 0;
    }
    MainWindow::ford_fulkerson(transport_network, flow, source, stock);
    for (QHash<Edge*, int>::const_iterator it = flow.cbegin(), end = flow.cend(); it != end; ++it) {
        if (it.value() >= 1 && comparison_edges.find(it.key()) != comparison_edges.end())
            matching.push_back(comparison_edges[it.key()]);
    }

    delete source;
    delete stock;
    for (QHash<Edge*, int>::const_iterator it = flow.cbegin(), end = flow.cend(); it != end; ++it) {
        delete it.key();
    }
}

bool MainWindow::check_simple(QHash<Node*, QVector<Edge*>> &graph) {
    for (QList<Edge *> edg: graph) {
        for (auto e: edg) {
            if (!graph[e->get_to()].isEmpty()) return false;
        }
    }
    return true;
}

bool MainWindow::dfs(QHash<Node*,QVector<Edge*>> &graph, Node* source, Node* stock,
         QVector<Edge*> &path, QVector<Node*> &visited, QHash<Edge*, int> &flow) {
    if (source == stock) return true;
    if (std::find(visited.begin(), visited.end(), source) != visited.end())
        return false;
    for (auto e: graph[source]) {
        if (flow[e] >= 1) continue;
        visited.push_back(source);
        if (dfs(graph, e->get_to(), stock, path, visited, flow)) {
            path.push_back(e);
            return true;
        }
    }
    return false;
}

Edge* MainWindow::get_edge(QHash<Node*,QVector<Edge*>> &graph, Node* from, Node* to) {
    for (auto e: graph[from]) {
        if (e->get_to() == to)
            return e;
    }
    return nullptr;
}

void MainWindow::ford_fulkerson(QHash<Node*,QVector<Edge*>> &graph, QHash<Edge*, int> &flow, Node* source, Node* stock) {
    while (true) {
        QVector<Edge*> path;
        QVector<Node*> visited;
        if (!dfs(graph, source, stock, path, visited, flow))
            break;
        for (auto e: path) {
            flow[e] += 1;
            auto antiEdge = get_edge(graph, e->get_to(), e->get_from());
            if (antiEdge != nullptr)
                flow[antiEdge] -= 1;
        }
    }
}