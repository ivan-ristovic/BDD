#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bdd.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bdd(nullptr)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->pbShow, SIGNAL(clicked()), this, SLOT(drawTree()));
    connect(ui->cbV1, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV2, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV3, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV4, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV5, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV6, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV7, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV8, SIGNAL(clicked()), this, SLOT(updateTree()));

    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    ui->pbReduce->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete scene;
    delete ui;
}

void MainWindow::drawTree()
{
    // create tree
    m_bdd = new BDDNode(0, 0, 1);
    m_bdd->insert(2);
    m_bdd->insert(3);

    // draw
    scene->clear();
    m_bdd->draw(scene);
    updateTree();

    ui->pbReduce->setEnabled(true);
}

void MainWindow::updateTree()
{
    if (m_bdd == nullptr)
        return;

    std::vector<const QCheckBox *> checkboxes;
    checkboxes.reserve(8);
    checkboxes.push_back(ui->cbV1);
    checkboxes.push_back(ui->cbV2);
    checkboxes.push_back(ui->cbV3);
    checkboxes.push_back(ui->cbV4);
    checkboxes.push_back(ui->cbV5);
    checkboxes.push_back(ui->cbV6);
    checkboxes.push_back(ui->cbV7);
    checkboxes.push_back(ui->cbV8);

    std::vector<bool> valueStack;

    m_bdd->updateValues(valueStack, checkboxes);
}

void MainWindow::reduce()
{
    ui->pbReduce->setEnabled(false);

    // add tree reduce logic here
}
