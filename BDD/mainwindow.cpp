#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    m_bdd = std::make_shared<BDDNode>();
    // create tree

    // draw
}
