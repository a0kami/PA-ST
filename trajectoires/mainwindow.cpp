#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <math.h>


float distEucl(Json::Value p1, Json::Value p2) {
    return sqrt((p1["x"].asFloat() - p2["x"].asFloat()) * (p1["x"].asFloat() - p2["x"].asFloat())
         + (p1["y"].asFloat() - p2["y"].asFloat()) * (p1["y"].asFloat() - p2["y"].asFloat()));
}

float dtw(Json::Value t1, Json::Value t2) {
    //initialisation du tableau
    std::vector<std::vector<float>> dist(t1.size()+1, std::vector<float>(t2.size()+1, 0));
    //std::cout << "dimention du tableau : [" << dist.size() << "," << dist[0].size() << "]" << std::endl;

    for(uint x = 1 ; x < t1.size()+1; x++) {
        for(uint y = 1 ; y < t2.size()+1; y++) {
            float poidChemin = std::min(std::min(dist[x-1][y] , dist[x][y-1]), dist[x][y]);
            dist[x][y] = distEucl(t1[x-1], t2[y-1]) + poidChemin;
        }
    }

    return dist[t1.size()][t2.size()];
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    ui->horizontalSlider->setMaximum(queries.size() - 1);

    /*
    //test lecture
    std::cout << "trajectoire " << trajectoires[0]["id"]
            << ", echantillon 0 : [" << trajectoires[0]["track"][0]["x"].asString()
            << "," << trajectoires[0]["track"][0]["y"].asString() << "]" << std::endl;
    */

    //test symetrie
    std::cout << "distance entre t0 et t1 : " << dtw(trajectoires[0]["track"], trajectoires[1]["track"]) << std::endl;
    std::cout << "distance entre t1 et t0 : " << dtw(trajectoires[1]["track"], trajectoires[0]["track"]) << std::endl;

    // mesure des distances
    for(uint i = 0 ; i < queries.size(); i++) {
        distances.push_back(std::vector<float>());
        for(uint j = 0 ; j < trajectoires.size(); j++) {
            distances[i].push_back(dtw(queries[i], trajectoires[j]["track"]));
            std::cout << "distance entre q" << i << " et t" << j << " : " << distances[i][j] << std::endl;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    int px, py;
    uint sldval = ui->horizontalSlider->value();
    //painter.drawLine(this->width() * 0.1, this->height() * 0.9, this->width() * 0.9, this->height() * 0.1);

    //calcul de la trajectoire avec la plus petite dtw
    uint ind = 0;
    float dist = distances[sldval][0];
    for(uint i = 1 ; i < distances[sldval].size(); i++) {
        if(dist > distances[sldval][i]) {
            ind = i;
            dist = distances[sldval][i];
        }
    }

    //affichage des trajectoires
    //Les x sont compris dans [0, 960] et les y dans [0, 540].
    //L'axe des y pointe vers le bas.
    for(uint i = 1 ; i < trajectoires.size(); i++) {
        if(i != ind) {
            painter.setPen(QPen(QColor(180,220,220), 2, Qt::DashLine, Qt::RoundCap));
            for(uint j = 1 ; j < trajectoires[i]["track"].size(); j++) {
                int x1 = trajectoires[i]["track"][j-1]["x"].asInt();
                int x2 = trajectoires[i]["track"][j]["x"].asInt();
                int y1 = trajectoires[i]["track"][j-1]["y"].asInt();
                int y2 = trajectoires[i]["track"][j]["y"].asInt();
                x1 = (x1 + 20) * this->width() / 1000;
                x2 = (x2 + 20) * this->width() / 1000;
                y1 = (y1 + 20) * this->height() / 660;
                y2 = (y2 + 20) * this->height() / 660;
                painter.drawLine(x1, y1, x2, y2);
            }
            //point sur le debut de la trajectoire
            painter.setPen(QPen(QColor(180,220,220), 12, Qt::DashLine, Qt::RoundCap));
            px = trajectoires[i]["track"][0]["x"].asInt();
            px = (px + 20) * this->width() / 1000;
            py = trajectoires[i]["track"][0]["y"].asInt();
            py = (py + 20) * this->height() / 660;
            painter.drawPoint(px,py);
        }
    }

    //affichage de la trajectoire la plus proche en dernière
    //pour qu'elle soit par dessus les autres
    painter.setPen(QPen(QColor(0,150,150), 2, Qt::DashLine, Qt::RoundCap));
    for(uint j = 1 ; j < trajectoires[ind]["track"].size(); j++) {
        int x1 = trajectoires[ind]["track"][j-1]["x"].asInt();
        int x2 = trajectoires[ind]["track"][j]["x"].asInt();
        int y1 = trajectoires[ind]["track"][j-1]["y"].asInt();
        int y2 = trajectoires[ind]["track"][j]["y"].asInt();
        x1 = (x1 + 20) * this->width() / 1000;
        x2 = (x2 + 20) * this->width() / 1000;
        y1 = (y1 + 20) * this->height() / 660;
        y2 = (y2 + 20) * this->height() / 660;
        painter.drawLine(x1, y1, x2, y2);
    }
    //point sur le debut de la trajectoire
    painter.setPen(QPen(QColor(0,150,150), 12, Qt::DashLine, Qt::RoundCap));
    px = trajectoires[ind]["track"][0]["x"].asInt();
    px = (px + 20) * this->width() / 1000;
    py = trajectoires[ind]["track"][0]["y"].asInt();
    py = (py + 20) * this->height() / 660;
    painter.drawPoint(px,py);

    //affichage de la query
    painter.setPen(QPen(QColor(255,0,0), 2, Qt::DashLine, Qt::RoundCap));
    for(uint j = 1 ; j < queries[sldval].size(); j++) {
        int x1 = queries[sldval][j-1]["x"].asInt();
        int x2 = queries[sldval][j]["x"].asInt();
        int y1 = queries[sldval][j-1]["y"].asInt();
        int y2 = queries[sldval][j]["y"].asInt();
        x1 = (x1 + 20) * this->width() / 1000;
        x2 = (x2 + 20) * this->width() / 1000;
        y1 = (y1 + 20) * this->height() / 660;
        y2 = (y2 + 20) * this->height() / 660;
        painter.drawLine(x1, y1, x2, y2);
    }
    //point sur le debut de la query
    painter.setPen(QPen(QColor(255,0,0), 12, Qt::DashLine, Qt::RoundCap));
    px = queries[sldval][0]["x"].asInt();
    px = (px + 20) * this->width() / 1000;
    py = queries[sldval][0]["y"].asInt();
    py = (py + 20) * this->height() / 660;
    painter.drawPoint(px,py);
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
    //std::cout << value << std::endl;
    this->update();
}
