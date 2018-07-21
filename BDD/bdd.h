#ifndef BDD_H
#define BDD_H

#include <memory>
#include <iostream>
#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsObject>
#include <QPainter>
#include <QCheckBox>

using Variable = unsigned;

class BDDNode : public QGraphicsObject
{
    Q_OBJECT

private:
    static constexpr int X_OFFSET = 100;
    static constexpr int Y_OFFSET = 50;
    static constexpr int NODE_RADIUS = 25;

public:
    BDDNode(qreal x, qreal y, Variable v);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void insert(Variable v, bool highValue = true, bool lowValue = false);
    void draw(QGraphicsScene *scene);
    void updateValues(std::vector<bool> &values, const std::vector<const QCheckBox *> &checkboxes);

private:

    void insertInternal(Variable v, bool highValue, bool lowValue, unsigned level);

    BDDNode *m_low;
    BDDNode *m_high;
    std::atomic<unsigned> m_depth;
    Variable m_var;
    bool m_value;
};

#endif // BDD_H
