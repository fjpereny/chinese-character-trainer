#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionClose_triggered();

    void on_actionAbout_triggered();

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;

    std::vector<int> *mouse_drag_points_x;
    std::vector<int> *mouse_drag_points_y;
};
#endif // MAINWINDOW_H
