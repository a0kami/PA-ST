#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>

enum TypeDist {Dtw, Sdtw, Hausdorff};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->frame, SIGNAL(query_added()), this, SLOT(on_query_added()));

    QString url = "data/img/camera2.png";
    image = QImage(url,"PNM");

    /*
    Format query :
    [int][int]["x"|"y"]

    Format tracks :
    [int]["id"]
    [int]["track"][int]["x"|"y"|"time"]
    */
    std::ifstream ifs_quer("data/queriesFull.json");
    std::ifstream ifs_traj("data/tracksWithTimes.json");
    ifs_quer >> queries;
    ifs_traj >> trajectoires;

    //remplissage de la liste déroulante de queries
    ui->comboBox_queries->clear();
    for(uint i = 0; i < queries.size(); i++) {
        ui->comboBox_queries->addItem(QString::number(i));
    }

    calcDist = &dtw;
    // mesure des distances
    for(uint j = 0 ; j < trajectoires.size(); j++) {
        distances.push_back(calcDist(queries[0], trajectoires[j]["track"]));
    }

    loading = false;
    on_comboBox_queries_currentIndexChanged(0);

    //envoi à l'affichage
    ui->frame->setData(&image, &queries, &trajectoires, &distances);
    ui->frame->setCurrentQuery(ui->comboBox_queries->currentIndex());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_img_stateChanged(int arg1)
{
    ui->frame->setDrawImage(arg1 == Qt::Checked);
}

void MainWindow::on_checkBox_traj_stateChanged(int arg1)
{
    ui->frame->setDrawDistTraj(arg1 == Qt::Checked);
}

void MainWindow::on_comboBox_queries_currentIndexChanged(int index)
{
    if(loading)
        return;

    // Mesure des distances
    distances.clear();
    for(uint j = 0 ; j < trajectoires.size(); j++) {
        distances.push_back(calcDist(queries[index], trajectoires[j]["track"]));
    }

    // Mise à jour de l'interface
    ui->frame->setCurrentQuery(index);

    // Tri des distances par ordre croissant
    std::vector<std::pair<float, uint>> distanceRank;
    for(uint i = 0; i < distances.size(); ++i) {
        distanceRank.push_back(std::pair<float, uint>(distances[i], i));
    }
    std::sort(distanceRank.begin(), distanceRank.end(), [](std::pair<float, uint> a, std::pair<float, uint> b) {
        return a.first <= b.first;
    }
    );

    // Ajout des lignes dans la table
    ui->tableDist->setRowCount(0);
    QStringList labels;
    for(auto distanceItem: distanceRank) {
        labels.append("");
        ui->tableDist->insertRow(ui->tableDist->rowCount());
        ui->tableDist->setItem(ui->tableDist->rowCount()-1, 0, new QTableWidgetItem("Trajectoire " + QString::number(distanceItem.second)));
        ui->tableDist->setItem(ui->tableDist->rowCount()-1, 1, new QTableWidgetItem(QString::number(int(distanceItem.first))));
    }
    ui->tableDist->setVerticalHeaderLabels(labels);
}

void MainWindow::on_comboBox_distance_currentIndexChanged(int index)
{
    switch(index) {
    case TypeDist::Dtw :
        calcDist = &dtw;
        on_comboBox_queries_currentIndexChanged(ui->comboBox_queries->currentIndex());
        return;
    case TypeDist::Sdtw :
        calcDist = &sdtw;
        on_comboBox_queries_currentIndexChanged(ui->comboBox_queries->currentIndex());
        return;
    case TypeDist::Hausdorff :
        calcDist = &hausdorff;
        on_comboBox_queries_currentIndexChanged(ui->comboBox_queries->currentIndex());
    default:
        return;
    }
}

void MainWindow::on_query_added()
{
    ui->comboBox_queries->addItem(QString::number(ui->comboBox_queries->count()));
    ui->comboBox_queries->setCurrentIndex(ui->comboBox_queries->count()-1);
    on_comboBox_queries_currentIndexChanged(ui->comboBox_queries->count()-1);
}
