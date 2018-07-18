#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bdd.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->pbShow, SIGNAL(clicked()), this, SLOT(drawTree()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawTree()
{
    // create tree
    m_bdd = std::make_shared<BDDNode>(1);
    m_bdd->insert(2);
    m_bdd->insert(3);

    std::cout << m_bdd << std::endl;

    // draw
}
