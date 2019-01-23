#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

#include <trajdist.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkBox_img_stateChanged(int arg1);
    void on_checkBox_traj_stateChanged(int arg1);
    void on_comboBox_queries_currentIndexChanged(int index);
    //void updateDistTable(const QString&);
    
    void on_comboBox_distance_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    QImage image;
    Json::Value queries;
    Json::Value trajectoires;
    std::vector<float> distances;
    bool loading = true;
    float (*calcDist)(Json::Value t1, Json::Value t2);

};

#endif // MAINWINDOW_H
