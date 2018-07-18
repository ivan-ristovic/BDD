#ifndef NODEITEM_H
#define NODEITEM_H

#include <QObject>
#include <QGraphicsObject>
#include <QPainter>
#include <memory>

class NodeItem : public QGraphicsObject
{
    Q_OBJECT

public:
    NodeItem(qreal x, qreal y, unsigned var, bool val);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

private:
    unsigned m_var;
    bool m_val;
};

using NodeGraphic = std::shared_ptr<NodeItem>;

#endif // NODEITEM_H
