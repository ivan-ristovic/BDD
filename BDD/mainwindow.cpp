#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bdd.h"
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bdd(nullptr)
    , m_reduceTicker(new QTimer())
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(drawTree()));
    connect(ui->cbV1, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV2, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV3, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV4, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV5, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV6, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV7, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->cbV8, SIGNAL(clicked()), this, SLOT(updateTree()));
    connect(ui->pbReduce, SIGNAL(clicked()), this, SLOT(reduceTree()));

    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    ui->pbReduce->setEnabled(true);

    connect(m_reduceTicker, SIGNAL(timeout()), this, SLOT(reduceTick()));
    m_reduceTicker->setInterval(1000);

    drawTree();
}

MainWindow::~MainWindow()
{
    delete m_reduceTicker;
    delete scene;
    delete ui;
}

void MainWindow::drawTree()
{
    m_reduceTicker->stop();

    scene->clear();
    ui->cbV1->setChecked(false);
    ui->cbV2->setChecked(false);
    ui->cbV3->setChecked(false);
    ui->cbV4->setChecked(false);
    ui->cbV5->setChecked(false);
    ui->cbV6->setChecked(false);
    ui->cbV7->setChecked(false);
    ui->cbV8->setChecked(false);
    ui->cbV1->setEnabled(true);
    ui->cbV2->setEnabled(true);
    ui->cbV3->setEnabled(true);
    ui->cbV4->setEnabled(true);
    ui->cbV5->setEnabled(true);
    ui->cbV6->setEnabled(true);
    ui->cbV7->setEnabled(true);
    ui->cbV8->setEnabled(true);

    // create tree
    m_bdd = new BDDNode(0, 0, 1);
    m_bdd->insert(2);
    m_bdd->insert(3);

    // draw
    m_bdd->draw(scene);
    updateTree();

    ui->pbReduce->setEnabled(true);
    ui->graphicsView->viewport()->update();
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

void MainWindow::reduceTree()
{
    ui->cbV1->setEnabled(false);
    ui->cbV2->setEnabled(false);
    ui->cbV3->setEnabled(false);
    ui->cbV4->setEnabled(false);
    ui->cbV5->setEnabled(false);
    ui->cbV6->setEnabled(false);
    ui->cbV7->setEnabled(false);
    ui->cbV8->setEnabled(false);

    m_reduceTicker->start();
}

void MainWindow::reduceTick()
{
    if (!m_bdd->reduceStep())
        m_reduceTicker->stop();
    ui->graphicsView->viewport()->update();
}
