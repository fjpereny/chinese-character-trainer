#include "source/mainwindow.h"
#include "ui_mainwindow.h"
#include "source/aboutdialog.h"

#include <iostream>
#include <vector>

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),

      mouse_drag_points_x(new std::vector<int>),
      mouse_drag_points_y(new std::vector<int>)
{
    ui->setupUi(this);

    mouse_drag_points_x->clear();
    mouse_drag_points_y->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionClose_triggered()
{
    this->close();
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog *d = new AboutDialog(this);
    d->show();
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mouse_pos = event->position();
    int x = mouse_pos.x();
    int y = mouse_pos.y();

    mouse_drag_points_x->push_back(x);
    mouse_drag_points_y->push_back(y);
    std::cout << "Mouse Point Count: " << mouse_drag_points_x->size() << std::endl;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    std::cout << "Mouse Pressed" << std::endl;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    int x_total = 0;
    if (mouse_drag_points_x->size() > 0)
    {
        for (int i=0; i<mouse_drag_points_x->size(); ++i)
        {
            x_total += mouse_drag_points_x->at(i);
        }
    }
    int y_total = 0;
    if (mouse_drag_points_y->size() > 0)
    {
        for (int i=0; i<mouse_drag_points_y->size(); ++i)
        {
            y_total += mouse_drag_points_y->at(i);
        }
    }

    double ave_x, ave_y;
    if (mouse_drag_points_x->size() > 0)
        ave_x = x_total / mouse_drag_points_x->size();
    else
        ave_x = 0;

    if (mouse_drag_points_y->size() > 0)
        ave_y = y_total / mouse_drag_points_y->size();
    else
        ave_y = 0;


    int max_x = 0, min_x = 0, max_y = 0, min_y = 0;
    if (mouse_drag_points_x->size() > 0 && mouse_drag_points_y->size() > 0)
    {
        max_x = mouse_drag_points_x->at(0);
        min_x = mouse_drag_points_x->at(0);
        for (int i=0; i<mouse_drag_points_x->size(); ++i)
        {
            if (mouse_drag_points_x->at(i) > max_x)
                max_x = mouse_drag_points_x->at(i);
            if (mouse_drag_points_x->at(i) < min_x)
                min_x = mouse_drag_points_x->at(i);
        }
        max_y = mouse_drag_points_y->at(0);
        min_y = mouse_drag_points_y->at(0);
        for (int i=0; i<mouse_drag_points_y->size(); ++i)
        {
            if (mouse_drag_points_y->at(i) > max_y)
                max_y = mouse_drag_points_y->at(i);
            if (mouse_drag_points_y->at(i) < min_y)
                min_y = mouse_drag_points_y->at(i);
        }
    }

    int delta_x, delta_y;
    delta_x = max_x - min_x;
    delta_y = max_y - min_y;
    double delta_x_y_ratio = (double)delta_x / (double)delta_y;

    std::string shape;
    if (delta_x_y_ratio >= 2.5 && delta_x >= 50)
        shape = "Horizontal Stroke";
    else if (delta_x_y_ratio <= 0.4 && delta_y >= 50)
        shape = "Vertical Stroke";
    else
        shape = "Dot";

    mouse_drag_points_x->clear();
    mouse_drag_points_y->clear();
    std::cout << "Mouse Released" << std::endl;
    std::cout << "Max X: " << max_x << std::endl;
    std::cout << "Min X: " << min_x << std::endl;
    std::cout << "Max Y: " << max_y << std::endl;
    std::cout << "Min Y: " << min_y << std::endl;
    std::cout << "Delta X: " << delta_x << std::endl;
    std::cout << "Delta Y: " << delta_y << std::endl;
    std::cout << "Delta X/Y Ratio: " << delta_x_y_ratio << std::endl;
    std::cout << "Average X: " << ave_x << std::endl;
    std::cout << "Average Y: " << ave_y << std::endl;
    std::cout << "Shape: " << shape << std::endl;

    ui->strokeLabel->setText(QString::fromStdString(shape));
}
