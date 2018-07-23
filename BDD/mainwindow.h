#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "bdd.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void drawTree();
    void updateTree();
    void reduceTree();
    void reduceTick();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    BDDNode *m_bdd;
    QTimer *m_reduceTicker;
};

#endif // MAINWINDOW_H
