#ifndef TREELISTDECORATOR_H
#define TREELISTDECORATOR_H

#include <QColor>
#include <QList>
#include <QObject>

class nodeListElement;
class treeListElement;

class TreeListDecorator : public QObject {
    Q_OBJECT
public:
    explicit TreeListDecorator(QObject *parent = 0);

public slots:
    QColor color(treeListElement *self);
    nodeListElement *first_node(treeListElement *self);
    QList<nodeListElement *> *nodes(treeListElement *self);
    int tree_id(treeListElement *self);
    QString comment(treeListElement *self);
    static QString static_Tree_help();
};

#endif // TREELISTDECORATOR_H
