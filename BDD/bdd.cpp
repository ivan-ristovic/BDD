#include "bdd.h"
#include <QBrush>
#include <QPen>
#include <QDebug>

BDDNode::BDDNode(qreal x, qreal y, Variable v, bool highValue, bool lowValue)
    : m_low(nullptr)
    , m_high(nullptr)
    , m_depth(0)
    , m_var(v)
{
    setPos(x, y);

    // if this is a variable node, automatically add leaves as children
    if (m_var) {
        m_high = new BDDNode(x + X_OFFSET / m_var, y + Y_OFFSET, 0);
        m_low = new BDDNode(x - X_OFFSET / m_var, y + Y_OFFSET, 0);
        m_high->m_value = highValue;
        m_low->m_value = lowValue;
    }
}

void BDDNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QBrush b;
    m_value ? b = Qt::yellow : b = Qt::red;

    if (m_var) {
        QPen pen = QPen(QBrush(Qt::GlobalColor::blue), 2);
        painter->setPen(pen);
        painter->drawLine(NODE_RADIUS / 2, NODE_RADIUS / 2, m_high->x() - x() + NODE_RADIUS / 2, m_high->y() - y() + NODE_RADIUS / 2);
        pen.setStyle(Qt::PenStyle::DotLine);
        painter->setPen(pen);
        painter->drawLine(NODE_RADIUS / 2, NODE_RADIUS / 2, m_low->x() - x() + NODE_RADIUS / 2, m_low->y() - y() + NODE_RADIUS / 2);
        painter->setBrush(QBrush(Qt::yellow));
        painter->setPen(Qt::PenStyle::SolidLine);
        painter->drawEllipse(0, 0, NODE_RADIUS, NODE_RADIUS);
        painter->drawText(5, 18, QString::fromStdString("X" + std::to_string(m_var)));
    } else {
        painter->setBrush(b);
        painter->drawEllipse(0, 0, NODE_RADIUS, NODE_RADIUS);
        painter->drawText(8, 18, QString::fromStdString(m_value ? "T" : "F"));
    }
}

QRectF BDDNode::boundingRect() const
{
    return QRectF(0, 0, NODE_RADIUS, NODE_RADIUS);
}

QPainterPath BDDNode::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, NODE_RADIUS, NODE_RADIUS);
    return path;
}

void BDDNode::insert(Variable v, bool highValue, bool lowValue)
{
    insertInternal(v, highValue, lowValue, m_depth);
    m_depth.fetch_add(1);
}

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

void BDDNode::draw(QGraphicsScene *scene)
{
    scene->addItem(this);
    if (m_var) {
        m_low->draw(scene);
        m_high->draw(scene);
    }
}

void BDDNode::updateValues(std::vector<bool> &values, const std::vector<const QCheckBox *> &checkboxes)
{
    if (m_var) {
        values.push_back(false);
        m_low->updateValues(values, checkboxes);
        values.pop_back();
        values.push_back(true);
        m_high->updateValues(values, checkboxes);
        values.pop_back();
    } else {
        m_value = checkboxes[4 * values[0] + 2 * values[1] + values[2]]->isChecked();
        update();
        qDebug() << values[0] << '\t' << values[1] << '\t' << values[2] << "\t :" << m_value;
    }
}

void BDDNode::insertInternal(Variable v, bool highValue, bool lowValue, unsigned level)
{
    // check if we are at correct level
    if (level == 0) {
        // if yes, add new variable node to both children pointers
        m_high = new BDDNode(x() + X_OFFSET / m_var, y() + Y_OFFSET, v, highValue, lowValue);
        m_low = new BDDNode(x() - X_OFFSET / m_var, y() + Y_OFFSET, v, highValue, lowValue);
    } else {
        // if not, recursively insert into both subtrees
        m_high->insertInternal(v, highValue, lowValue, level - 1);
        m_low->insertInternal(v, highValue, lowValue, level - 1);
    }
}

std::ostream &operator<<(std::ostream &out, const BDDNode *bdd)
{
    return bdd->print(out);
}
