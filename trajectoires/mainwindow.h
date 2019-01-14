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
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *event);

    QImage image;
    Json::Value queries;
    Json::Value trajectoires;
    std::vector<std::vector<float>> distances;

};

#endif // MAINWINDOW_H
