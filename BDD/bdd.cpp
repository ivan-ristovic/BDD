#include "bdd.h"
#include <QBrush>
#include <QPen>
#include <QThread>
#include <QPair>
#include <QDebug>

BDDNode::BDDNode(qreal x, qreal y, Variable v)
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
        m_high->m_value = false;
        m_low->m_value = false;
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
    }
}

void BDDNode::insertInternal(Variable v, bool highValue, bool lowValue, unsigned level)
{
    // check if we are at correct level
    if (level == 0) {
        // if yes, add new variable node to both children pointers
        m_high = new BDDNode(x() + X_OFFSET / m_var, y() + Y_OFFSET, v);
        m_low = new BDDNode(x() - X_OFFSET / m_var, y() + Y_OFFSET, v);
    } else {
        // if not, recursively insert into both subtrees
        m_high->insertInternal(v, highValue, lowValue, level - 1);
        m_low->insertInternal(v, highValue, lowValue, level - 1);
    }
}


void BDDNode::reduce()
{
    bool bddChanged = true;
    while (bddChanged) // TODO figure out how to update the scene every sec
    {
        bddChanged = this->merge(this);
        if (!bddChanged)
        {
            bddChanged = this->eliminate(nullptr);
        }

        if (bddChanged)
        {
            // If there was a change, update the bddin 1s and have simulaton of reduction algorithm.
            this->scene()->update();
            QThread::sleep(1);
        }
    }
}


bool BDDNode::merge(BDDNode* root)
{
    QPair<BDDNode*, BDDNode*> i = this->findIsomorph(root, nullptr);
    if (i.first != nullptr)
    {
        // TODO free subtree
        if (i.second->m_low == i.first)
        {
            // free(i.second->m_low)
            i.second->m_low->hideSubTree();
            i.second->m_low = this;
        }
        else
        {
            // free(i.second->m_high)
            i.second->m_high->hideSubTree();
            i.second->m_high = this;
        }
        // THINK: pointer to parent?
        return true;
    }

    if (m_var)
    {
        bool l = m_low->merge(root);
        bool h = false;
        if (!l)
            h = m_high->merge(root);
        return l || h;
    }

    return false;
}

QPair<BDDNode*, BDDNode*> BDDNode::findIsomorph(BDDNode* root, BDDNode* parent)
{
    QPair<BDDNode*, BDDNode*> result(nullptr, nullptr);

    if (parent != nullptr && isIsomorph(root))
    {
        result.first = root;
        result.second = parent;
        return result;
    }
    if (root->m_var != 0)
    {
        result = findIsomorph(root->m_low, root);
        if (result.first == nullptr)
            result = findIsomorph(root->m_high, root);
    }
    return result;
}


bool BDDNode::isIsomorph(BDDNode* other)
{
    // same node
    if (this == other)
        return false;
    if (other == nullptr)
        return false;
    // vars are different
    if (m_var != other->m_var)
        return false;
    // leafs
    if (m_var == 0)
    {
        if (other->m_var == 0 && m_value == other->m_value)
            return true;
        else
            return false;
    }

    if (m_var)
    {
        bool l = this->m_low->isIsomorph(other->m_low);
        bool r = false;
        if (l)
            r = this->m_high->isIsomorph(other->m_high);

        return l && r;
    }

    return false;
}

bool BDDNode::eliminate(BDDNode* parent)
{
    if (this->m_var == 0)
        return false;

    if (this->m_low == this->m_high)
    {
        if (this->m_low->m_var != 0)
        {
            // TODO delete this->m_low
            this->m_low->hide();
            this->m_low = this->m_low->m_low;
            this->m_high = this->m_high->m_high;
        }
        else
        {
            if (parent == nullptr)
            {
                // TODO delete this->m_low
                this->m_low->hide();
                BDDNode* temp = this->m_low;
                this->m_var = temp->m_var;
                this->m_value = temp->m_value;
                this->m_low = nullptr;
                this->m_high = nullptr;
            }
            else
            {
                // TODO delete this
                this->hide();
                if (parent->m_low == this)
                    parent->m_low = this->m_low;
                else
                    parent->m_high = this->m_low;
            }
        }
        return true;
    }

    bool l = (this->m_low)->eliminate(this);
    bool h = false;
    if (!l)
        h = (this->m_high)->eliminate(this);

    return l || h;
}

bool BDDNode::operator==(const BDDNode& other) const
{
    if (typeid(*this) != typeid(other))
        return false;
    return m_var == other.m_var && (m_var == 0 ? m_value == other.m_value : true );
}

std::ostream& operator<< (std::ostream &out, BDDNode* const& node)
{
    if (node->m_var)
        out << "x" << "m_var";
    else
        out << (node->m_value ? "T" : "F");
    return out;
}

void BDDNode::hideSubTree()
{
    this->hide();
    if (this->m_low != nullptr)
        this->m_low->hideSubTree();
    if (this->m_high != nullptr)
        this->m_high->hideSubTree();
}
