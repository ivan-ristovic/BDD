#include "nodeitem.h"
#include <QBrush>

NodeItem::NodeItem(qreal x, qreal y, unsigned var, bool val)
    : m_var(var)
    , m_val(val)
{
    setPos(x, y);
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(QBrush(Qt::yellow));
    painter->drawEllipse(0, 0, 25, 25);
    if (m_var) {
        painter->drawText(5, 18, QString::fromStdString("X" + std::to_string(m_var)));
    } else {
        painter->drawText(8, 18, QString::fromStdString(m_val ? "T" : "F"));
    }
}

QRectF NodeItem::boundingRect() const
{
    return QRectF(0, 0, 25, 25);
}

QPainterPath NodeItem::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, 25, 25);
    return path;
}
