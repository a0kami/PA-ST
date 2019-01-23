#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    ui->comboBox_queries->clear();
    for(int i = 0; i < queries.size(); i++) {
        ui->comboBox_queries->addItem(QString::number(i));
    }

    /*
    //test lecture
    std::cout << "trajectoire " << trajectoires[0]["id"]
            << ", echantillon 0 : [" << trajectoires[0]["track"][0]["x"].asString()
            << "," << trajectoires[0]["track"][0]["y"].asString() << "]" << std::endl;
    */

    //test symetrie
    std::cout << "distance entre t0 et t1 : " << dtw(trajectoires[0]["track"], trajectoires[1]["track"]) << std::endl;
    std::cout << "distance entre t1 et t0 : " << dtw(trajectoires[1]["track"], trajectoires[0]["track"]) << std::endl;
//  std::cout << "distance entre t0 et t1 : " << sdtw(trajectoires[0]["track"], trajectoires[1]["track"]) << std::endl;
//  std::cout << "distance entre t1 et t0 : " << sdtw(trajectoires[1]["track"], trajectoires[0]["track"]) << std::endl;
    // mesure des distances
    for(uint i = 0 ; i < queries.size(); i++) {
        distances.push_back(std::vector<float>());
        for(uint j = 0 ; j < trajectoires.size(); j++) {
            distances[i].push_back(dtw(queries[i], trajectoires[j]["track"]));
            std::cout << "distance entre q" << i << " et t" << j << " : " << distances[i][j] << std::endl;
        }
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

    ui->frame->setCurrentQuery(index);
    ui->tableDist->setRowCount(0);
    QStringList labels;
    for(uint i = 0 ; i < distances[index].size(); i++) {
        labels.append("");
        ui->tableDist->insertRow(ui->tableDist->rowCount());
        ui->tableDist->setItem(ui->tableDist->rowCount()-1, 0, new QTableWidgetItem("Trajectoire " + QString::number(i)));
        ui->tableDist->setItem(ui->tableDist->rowCount()-1, 1, new QTableWidgetItem(QString::number(int(distances[index][i]))));
    }
    ui->tableDist->setVerticalHeaderLabels(labels);
}
