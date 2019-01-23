#ifndef DISPLAYAREA_H
#define DISPLAYAREA_H

#include <QFrame>
#include <QPainter>
#include <QMouseEvent>

#include <dist/json/json.h>

struct Pt {
    int x;
    int y;
};

class DisplayArea : public QFrame {
Q_OBJECT
public:
    explicit DisplayArea(QWidget *parent = nullptr);

    void setData(QImage* imageP, Json::Value* queriesP, Json::Value* trajectoiresP, std::vector<float>* distancesP);
    void setCurrentQuery(int value);
    void setDrawImage(bool value);
    void setDrawDistTraj(bool value);

signals:
    void query_added();

private:
    QImage* image = nullptr;
    Json::Value* queries = nullptr;
    Json::Value* trajectoires = nullptr;
    std::vector<float>* distances = nullptr;
    uint currentQuery = 0;
    bool drawImage = true;
    bool drawDistTraj = true;

    void paintEvent(QPaintEvent *event);

    bool isDrawing = false;
    std::vector<Pt> drawing;
    double drawstep = 60*60;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // DISPLAYAREA_H
