#include "bdd.h"
#include <QBrush>
#include <QPen>
#include <QThread>
#include <QPair>
#include <QDebug>


QColor BDDNode::BLUE = QColor(178, 215, 229);
QColor BDDNode::GREEN = QColor(102, 204, 121);
QColor BDDNode::YELLOW = QColor(255, 248, 127);
QColor BDDNode::RED = QColor(255, 102, 102);
QColor BDDNode::ORANGE = QColor(255, 171, 25);

BDDNode::BDDNode(qreal x, qreal y, Variable v, BDDNode* parent)
    : m_low(nullptr)
    , m_high(nullptr)
    , m_depth(0)
    , m_var(v)
    , m_isRoot(false)
{
    setPos(x, y);

    if (parent != nullptr)
        m_parents.push_back(parent);
    else
        m_isRoot = true;

    // if this is a variable node, automatically add leaves as children
    if (m_var) {
        m_high = new BDDNode(x + X_OFFSET / m_var, y + Y_OFFSET, 0, this);
        m_low = new BDDNode(x - X_OFFSET / m_var, y + Y_OFFSET, 0, this);
        m_high->m_value = false;
        m_low->m_value = false;
    }
}

void BDDNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QBrush b;
    m_value ? b = YELLOW : b = ORANGE;

    if (m_var) {
        QPen pen = QPen(QBrush(BLUE), 2);
        painter->setPen(pen);
        painter->drawLine(NODE_RADIUS / 2, NODE_RADIUS / 2, m_high->x() - x() + NODE_RADIUS / 2, m_high->y() - y() + NODE_RADIUS / 2);
        pen.setStyle(Qt::PenStyle::DotLine);
        painter->setPen(pen);
        painter->drawLine(NODE_RADIUS / 2, NODE_RADIUS / 2, m_low->x() - x() + NODE_RADIUS / 2, m_low->y() - y() + NODE_RADIUS / 2);
        painter->setBrush(QBrush(BLUE));
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
        m_high = new BDDNode(x() + X_OFFSET / m_var, y() + Y_OFFSET, v, this);
        m_low = new BDDNode(x() - X_OFFSET / m_var, y() + Y_OFFSET, v, this);
    } else {
        // if not, recursively insert into both subtrees
        m_high->insertInternal(v, highValue, lowValue, level - 1);
        m_low->insertInternal(v, highValue, lowValue, level - 1);
    }
}

void BDDNode::reduce()
{
    while (reduceStep())
        ;
}

bool BDDNode::reduceStep()
{
    bool bddChanged = this->merge(this);
    if (!bddChanged)
    {
        bddChanged = this->eliminate();
    }

    if (bddChanged)
    {
        // If there was a change, update the bdd in 1s and have simulaton of reduction algorithm.
        this->scene()->update();
    }

    return bddChanged;
}

bool BDDNode::merge(BDDNode* root)
{
    BDDNode* i = this->findIsomorph(root);
    if (i != nullptr)
    {        
        std::vector<BDDNode*> toDelete;
        for (BDDNode* parent : i->m_parents)
        {
            this->m_parents.push_back(parent);
            if (parent->m_low == i)
                parent->m_low = this;
            else
                parent->m_high = this;
            toDelete.push_back(parent);
        }

        for (BDDNode* el : toDelete)
            removeFromVector(i->m_parents, el);

        i->hideSubTree();

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

BDDNode* BDDNode::findIsomorph(BDDNode* root)
{
    if (!root->m_isRoot && isIsomorph(root))
    {
        return root;
    }

    BDDNode* result = nullptr;
    if (root->m_var != 0)
    {
        result = findIsomorph(root->m_low);
        if (result == nullptr)
            result = findIsomorph(root->m_high);
    }
    return result;
}

bool BDDNode::isIsomorph(BDDNode* other)
{
    if (other == nullptr)
        return false;
    // vars are different
    if (m_var != other->m_var)
        return false;
    // leafs
    if (m_var == 0)
    {
        if (this == other)
        {
            if (m_parents[0] != other->m_parents[0])
                return true;
        }
        else
        {
            if (other->m_var == 0 && m_value == other->m_value)
                return true;
        }
        return false;
    }
    // same node
    if (this == other)
        return false;

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

bool BDDNode::eliminate()
{
    if (this->m_var == 0)
        return false;

    if (this->m_low == this->m_high)
    {
        removeFromVector(this->m_low->m_parents, this);
        if (!this->m_parents.empty() || !this->m_isRoot)
        {
            std::vector<BDDNode*> toDelete;
            for (int i = 0; i < this->m_parents.size(); i++)
            {
                this->m_low->m_parents.push_back(this->m_parents[i]);
                if (this->m_parents[i]->m_low == this)
                    this->m_parents[i]->m_low = this->m_high;
                else
                    this->m_parents[i]->m_high = this->m_high;
                toDelete.push_back(this->m_parents[i]);
            }

            for (BDDNode* el : toDelete)
                removeFromVector(this->m_parents, el);
            this->hideSubTree();
        }
        else
        {
            BDDNode* temp = this->m_low;
            this->m_var = temp->m_var;
            this->m_value = temp->m_value;
            this->m_low = temp->m_low;
            this->m_high = temp->m_high;
            for (BDDNode* parent : temp->m_parents)
            {
                if (parent->m_low == temp)
                    parent->m_low = this;
                else
                    parent->m_high = this;
                //this->m_parents.push_back(parent);
            }
            temp->hide();
        }

        return true;
    }

    bool l = (this->m_low)->eliminate();
    bool h = false;
    if (!l)
        h = (this->m_high)->eliminate();

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
    if (!this->m_parents.empty() || this->m_isRoot)
        return;

    this->hide();
    if (this->m_low != nullptr)
    {
        removeFromVector(this->m_low->m_parents, this);
        this->m_low->hideSubTree();
    }
    if (this->m_high != nullptr)
    {
        removeFromVector(this->m_high->m_parents, this);
        this->m_high->hideSubTree();
    }
}

void BDDNode::removeFromVector(std::vector<BDDNode*>& parents, BDDNode* parent)
{
    parents.erase(std::remove_if(parents.begin(),
                                  parents.end(),
                                    [parent](BDDNode* n){return parent == n; }),
                   parents.end());
}


