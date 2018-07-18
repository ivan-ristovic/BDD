#include "bdd.h"

BDDNode::BDDNode(Variable v, bool highValue, bool lowValue)
    : m_low(nullptr)
    , m_high(nullptr)
    , m_var(v)
{
    // if this is a variable node, automatically add leaves as children
    if (m_var) {
        m_high = std::make_shared<BDDNode>(0);
        m_low = std::make_shared<BDDNode>(0);
        m_high->m_value = highValue;
        m_low->m_value = lowValue;
    }

}

void BDDNode::insert(Variable v, bool highValue, bool lowValue)
{
    insertInternal(v, highValue, lowValue, GlobalDepth);
    GlobalDepth.fetch_add(1);
}

#include <QDebug>
std::ostream &BDDNode::print(std::ostream &out) const
{
    qDebug() << "printing node with var: " << m_var;
    if (m_var) {
        m_low->print(out);
        out << " (x" << m_var << ") ";
        m_high->print(out);
    } else {
        out << (m_value ? "T" : "F");
    }
    return out;
}

void BDDNode::draw(QGraphicsScene *scene, qreal xpos, qreal ypos, unsigned level)
{
    m_node = std::make_shared<NodeItem>(xpos, ypos, m_var, m_value);
    scene->addItem(m_node.get());
    if (m_var) {
        m_high->draw(scene, xpos + 80 / level, ypos + 30, level + 1);
        m_low->draw(scene, xpos - 80 / level, ypos + 30, level + 1);
    }
}

void BDDNode::insertInternal(Variable v, bool highValue, bool lowValue, unsigned level)
{
    // check if we are at correct level
    if (level == 0) {
        // if yes, add new variable node to both children pointers
        m_high = std::make_shared<BDDNode>(v, highValue, lowValue);
        m_low = std::make_shared<BDDNode>(v, highValue, lowValue);
    } else {
        // if not, recursively insert into both subtrees
        m_high->insertInternal(v, highValue, lowValue, level - 1);
        m_low->insertInternal(v, highValue, lowValue, level - 1);
    }
}

std::ostream &operator<<(std::ostream &out, const BDD &bdd)
{
    return bdd->print(out);
}
