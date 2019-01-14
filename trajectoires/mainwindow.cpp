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
    int circleSize = 10;
    int squareSize = 8; //pls make this even ty (because I divide it by 2 ok)
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

    //draw le picture
    painter.drawImage(QRect(20 * this->width() / 1000, 20 * this->height() / 660,
                            this->width()-(40 * this->width() / 1000), this->height()-(120 * this->height() / 660)), image);

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
            painter.setPen(QPen(QColor(180,220,220), circleSize, Qt::DashLine, Qt::RoundCap));
            px = trajectoires[i]["track"][0]["x"].asInt();
            px = (px + 20) * this->width() / 1000;
            py = trajectoires[i]["track"][0]["y"].asInt();
            py = (py + 20) * this->height() / 660;
            painter.drawPoint(px,py);
            //square at the end
            painter.setPen(QPen(QColor(180,220,220), 2, Qt::SolidLine, Qt::RoundCap));
            px = trajectoires[i]["track"][trajectoires[i]["track"].size()-1]["x"].asInt();
            px = (px + 20) * this->width() / 1000;
            py = trajectoires[i]["track"][trajectoires[i]["track"].size()-1]["y"].asInt();
            py = (py + 20) * this->height() / 660;
            painter.drawRect(px-squareSize/2,py-squareSize/2,squareSize,squareSize);
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
    painter.setPen(QPen(QColor(0,150,150), circleSize, Qt::DashLine, Qt::RoundCap));
    px = trajectoires[ind]["track"][0]["x"].asInt();
    px = (px + 20) * this->width() / 1000;
    py = trajectoires[ind]["track"][0]["y"].asInt();
    py = (py + 20) * this->height() / 660;
    painter.drawPoint(px,py);
    //square at the end
    painter.setPen(QPen(QColor(0,150,150), 2, Qt::SolidLine, Qt::RoundCap));
    px = trajectoires[ind]["track"][trajectoires[ind]["track"].size()-1]["x"].asInt();
    px = (px + 20) * this->width() / 1000;
    py = trajectoires[ind]["track"][trajectoires[ind]["track"].size()-1]["y"].asInt();
    py = (py + 20) * this->height() / 660;
    painter.drawRect(px-squareSize/2,py-squareSize/2,squareSize,squareSize);

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
    painter.setPen(QPen(QColor(255,0,0), circleSize, Qt::DashLine, Qt::RoundCap));
    px = queries[sldval][0]["x"].asInt();
    px = (px + 20) * this->width() / 1000;
    py = queries[sldval][0]["y"].asInt();
    py = (py + 20) * this->height() / 660;
    painter.drawPoint(px,py);
    //square at the end
    painter.setPen(QPen(QColor(255,0,0), 2, Qt::SolidLine, Qt::RoundCap));
    px = queries[sldval][queries[sldval].size()-1]["x"].asInt();
    px = (px + 20) * this->width() / 1000;
    py = queries[sldval][queries[sldval].size()-1]["y"].asInt();
    py = (py + 20) * this->height() / 660;
    painter.drawRect(px-squareSize/2,py-squareSize/2,squareSize,squareSize);
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
    //std::cout << value << std::endl;
    this->update();
}
