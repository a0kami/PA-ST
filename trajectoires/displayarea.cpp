#include "displayarea.h"

DisplayArea::DisplayArea(QWidget *parent) : QFrame(parent) {

}

void DisplayArea::setData(QImage *imageP, Json::Value *queriesP, Json::Value *trajectoiresP, std::vector<float> *distancesP) {
    image = imageP;
    queries = queriesP;
    trajectoires = trajectoiresP;
    distances = distancesP;
}

void DisplayArea::setCurrentQuery(int value) {
    currentQuery = value;
    this->update();
}

void DisplayArea::setDrawImage(bool value) {
    drawImage = value;
    this->update();
}

void DisplayArea::setDrawDistTraj(bool value) {
    drawDistTraj = value;
    this->update();
}

void DisplayArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    int px, py, x1, x2, y1, y2;
    int circleSize = 10;
    int squareSize = 8; //pls make this even ty (because I divide it by 2 ok)

    //calcul de la trajectoire avec la plus petite distance
    uint ind = 0;
    float dist = (*distances)[0];
    for(uint i = 1 ; i < (*distances).size(); i++) {
        if(dist > (*distances)[i]) {
            ind = i;
            dist = (*distances)[i];
        }
    }

    //draw le picture
    if(drawImage)
        painter.drawImage(QRect(0, 0, this->width(), this->height()), *image);

    //affichage des trajectoires
    //Les x sont compris dans [0, 960] et les y dans [0, 540].
    //L'axe des y pointe vers le bas.
    if(drawDistTraj)
        for(uint i = 1 ; i < trajectoires->size(); i++) {
            if(i != ind) {
                painter.setPen(QPen(QColor(180,220,220), 2, Qt::DashLine, Qt::RoundCap));
                for(uint j = 1 ; j < (*trajectoires)[i]["track"].size(); j++) {
                    x1 = (*trajectoires)[i]["track"][j-1]["x"].asInt();
                    x2 = (*trajectoires)[i]["track"][j]["x"].asInt();
                    y1 = (*trajectoires)[i]["track"][j-1]["y"].asInt();
                    y2 = (*trajectoires)[i]["track"][j]["y"].asInt();
                    x1 = x1 * this->width() / image->width();
                    x2 = x2 * this->width() / image->width();
                    y1 = y1 * this->height() / image->height();
                    y2 = y2 * this->height() / image->height();
                    painter.drawLine(x1, y1, x2, y2);
                }
                //point sur le debut de la trajectoire
                painter.setPen(QPen(QColor(180,220,220), circleSize, Qt::DashLine, Qt::RoundCap));
                px = (*trajectoires)[i]["track"][0]["x"].asInt();
                px = px * this->width() / image->width();
                py = (*trajectoires)[i]["track"][0]["y"].asInt();
                py = py * this->height() / image->height();
                painter.drawPoint(px,py);
                //square at the end
                painter.setPen(QPen(QColor(180,220,220), 2, Qt::SolidLine, Qt::RoundCap));
                px = (*trajectoires)[i]["track"][(*trajectoires)[i]["track"].size()-1]["x"].asInt();
                px = px * this->width() / image->width();
                py = (*trajectoires)[i]["track"][(*trajectoires)[i]["track"].size()-1]["y"].asInt();
                py = py * this->height() / image->height();
                painter.drawRect(px-squareSize/2,py-squareSize/2,squareSize,squareSize);
            }
        }

    //affichage de la trajectoire la plus proche en dernière
    //pour qu'elle soit par dessus les autres
    painter.setPen(QPen(QColor(0,150,150), 2, Qt::DashLine, Qt::RoundCap));
    for(uint j = 1 ; j < (*trajectoires)[ind]["track"].size(); j++) {
        int x1 = (*trajectoires)[ind]["track"][j-1]["x"].asInt();
        int x2 = (*trajectoires)[ind]["track"][j]["x"].asInt();
        int y1 = (*trajectoires)[ind]["track"][j-1]["y"].asInt();
        int y2 = (*trajectoires)[ind]["track"][j]["y"].asInt();
        x1 = x1 * this->width() / image->width();
        x2 = x2 * this->width() / image->width();
        y1 = y1 * this->height() / image->height();
        y2 = y2 * this->height() / image->height();
        painter.drawLine(x1, y1, x2, y2);
    }

    //point sur le debut de la trajectoire
    painter.setPen(QPen(QColor(0,150,150), circleSize, Qt::DashLine, Qt::RoundCap));
    px = (*trajectoires)[ind]["track"][0]["x"].asInt();
    px = px * this->width() / image->width();
    py = (*trajectoires)[ind]["track"][0]["y"].asInt();
    py = py * this->height() / image->height();
    painter.drawPoint(px,py);
    //square at the end
    painter.setPen(QPen(QColor(0,150,150), 2, Qt::SolidLine, Qt::RoundCap));
    px = (*trajectoires)[ind]["track"][(*trajectoires)[ind]["track"].size()-1]["x"].asInt();
    px = px * this->width() / image->width();
    py = (*trajectoires)[ind]["track"][(*trajectoires)[ind]["track"].size()-1]["y"].asInt();
    py = py * this->height() / image->height();
    painter.drawRect(px-squareSize/2,py-squareSize/2,squareSize,squareSize);

    //affichage de la query
    painter.setPen(QPen(QColor(255,0,0), 2, Qt::DashLine, Qt::RoundCap));
    for(uint j = 1 ; j < (*queries)[currentQuery].size(); j++) {
        int x1 = (*queries)[currentQuery][j-1]["x"].asInt();
        int x2 = (*queries)[currentQuery][j]["x"].asInt();
        int y1 = (*queries)[currentQuery][j-1]["y"].asInt();
        int y2 = (*queries)[currentQuery][j]["y"].asInt();
        x1 = x1 * this->width() / image->width();
        x2 = x2 * this->width() / image->width();
        y1 = y1 * this->height() / image->height();
        y2 = y2 * this->height() / image->height();
        painter.drawLine(x1, y1, x2, y2);
    }
    //point sur le debut de la query
    painter.setPen(QPen(QColor(255,0,0), circleSize, Qt::DashLine, Qt::RoundCap));
    px = (*queries)[currentQuery][0]["x"].asInt();
    px = px * this->width() / image->width();
    py = (*queries)[currentQuery][0]["y"].asInt();
    py = py * this->height() / image->height();
    painter.drawPoint(px,py);
    //square at the end
    painter.setPen(QPen(QColor(255,0,0), 2, Qt::SolidLine, Qt::RoundCap));
    px = (*queries)[currentQuery][(*queries)[currentQuery].size()-1]["x"].asInt();
    px = px * this->width() / image->width();
    py = (*queries)[currentQuery][(*queries)[currentQuery].size()-1]["y"].asInt();
    py = py * this->height() / image->height();
    painter.drawRect(px-squareSize/2,py-squareSize/2,squareSize,squareSize);
}
