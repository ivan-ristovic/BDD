#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bdd.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->pbShow, SIGNAL(clicked()), this, SLOT(drawTree()));

    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete scene;
    delete ui;
}

void MainWindow::drawTree()
{
    // create tree
    m_bdd.reset();
    m_bdd = std::make_shared<BDDNode>(1);
    m_bdd->insert(2);
    m_bdd->insert(3);

    std::cout << m_bdd << std::endl;

    // draw
    scene->clear();
    m_bdd->draw(scene, 100, 0);
}
