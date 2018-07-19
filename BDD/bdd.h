#ifndef BDD_H
#define BDD_H

#include <memory>
#include <iostream>
#include <QGraphicsScene>
#include "nodeitem.h"

using Variable = unsigned;

class BDDNode;
using BDD = std::shared_ptr<BDDNode>;

class BDDNode : public std::enable_shared_from_this<BDDNode>
{
public:
    BDDNode(Variable v, bool highValue = true, bool lowValue = false);

    void insert(Variable v, bool highValue = true, bool lowValue = false);
    std::ostream& print(std::ostream& out) const;
    void draw(QGraphicsScene *scene, qreal xpos, qreal ypos, unsigned level = 1);

private:

    void insertInternal(Variable v, bool highValue, bool lowValue, unsigned level);

    bool m_value;
    BDD m_low;
    BDD m_high;
    Variable m_var;
    NodeItem *m_node;
    std::atomic<unsigned> m_depth;
};

std::ostream& operator<<(std::ostream& out, const BDD &bdd);

#endif // BDD_H
