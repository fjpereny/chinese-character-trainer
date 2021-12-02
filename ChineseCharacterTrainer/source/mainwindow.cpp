#include "source/mainwindow.h"
#include "ui_mainwindow.h"
#include "source/aboutdialog.h"

#include <iostream>
#include <vector>

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
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
    mouse_drag_points_x->clear();
    mouse_drag_points_y->clear();
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

    // Direction positive integer for up/right, negative for down/left
    int x_direction = 0;
    if (mouse_drag_points_x->size() > 1)
    {
        int x_diff = mouse_drag_points_x->at(mouse_drag_points_x->size()-1) - mouse_drag_points_x->at(0);
        if (x_diff > 0)
            x_direction = 1;
        else if (x_diff < 0)
            x_direction = -1;
    }
    int y_direction = 0;
    if (mouse_drag_points_y->size() > 1)
    {
        int y_diff = mouse_drag_points_y->at(mouse_drag_points_y->size()-1) - mouse_drag_points_y->at(0);
        if (y_diff > 0)
            y_direction = -1;
        else if (y_diff < 0)
            y_direction = 1;
    }

    // Line Path Segments
    int num_of_points_x = mouse_drag_points_x->size();
    int num_of_points_y = mouse_drag_points_y->size();

    int num_of_segments = 6;
    bool enough_x_points = num_of_points_x >= num_of_segments;
    bool enough_y_points = num_of_points_y >= num_of_segments;
    bool enough_points = enough_x_points && enough_y_points;

    int segment_interval_x = 0;
    int segment_interval_y = 0;
    if (num_of_segments != 0 && enough_points)
    {
        segment_interval_x = num_of_points_x / num_of_segments;
        segment_interval_y = num_of_points_y / num_of_segments;
    }

    std::vector<std::vector<int>> segments_x;
    std::vector<std::vector<int>> segments_y;
    if (enough_points)
    {
        for (int i=0; i<num_of_segments; ++i)
        {
        std::vector<int> segment_x(mouse_drag_points_x->begin() + (i * segment_interval_x),
                                   mouse_drag_points_x->begin() + ((i + 1) * segment_interval_x));
        std::vector<int> segment_y(mouse_drag_points_y->begin() + (i * segment_interval_y),
                                   mouse_drag_points_y->begin() + ((i + 1) * segment_interval_y));

        segments_x.push_back(segment_x);
        segments_y.push_back(segment_y);
        }
    }

    // Segment Min/Max/Delta
    std::vector<int> segments_x_max;
    std::vector<int> segments_y_max;
    std::vector<int> segments_x_min;
    std::vector<int> segments_y_min;
    std::vector<int> segments_x_delta;
    std::vector<int> segments_y_delta;
    if (enough_points)
    {
        for (int i=0; i<segments_x.size(); ++i)
        {
            int max = segments_x.at(i).at(0);
            int min = segments_x.at(i).at(0);
            for (int j=0; j<segments_x.at(i).size(); ++j)
            {
                if (segments_x.at(i).at(j) > max)
                    max = segments_x.at(i).at(j);
                if (segments_x.at(i).at(j) < min)
                    min = segments_x.at(i).at(j);
            }
            segments_x_max.push_back(max);
            segments_x_min.push_back(min);
            segments_x_delta.push_back(max - min);
        }
        for (int i=0; i<segments_y.size(); ++i)
        {
            int max = segments_y.at(i).at(0);
            int min = segments_y.at(i).at(0);
            for (int j=0; j<segments_y.at(i).size(); ++j)
            {
                if (segments_y.at(i).at(j) > max)
                    max = segments_y.at(i).at(j);
                if (segments_y.at(i).at(j) < min)
                    min = segments_y.at(i).at(j);
            }
            segments_y_max.push_back(max);
            segments_y_min.push_back(min);
            segments_y_delta.push_back(max - min);
        }
    }

    // Segmenta Delta X/Y Ratio
    std::vector<double> segments_delta_XY_ratio;
    if (enough_points)
    {
        for (int i=0; i<segments_x_delta.size(); ++i)
        {
            double ratio = ((double)segments_x_delta.at(i)) / ((double)segments_y_delta.at(i));
            segments_delta_XY_ratio.push_back(ratio);
        }
    }

    // Segment Direction
    std::vector<int> segment_directions_x;
    std::vector<int> segment_directions_y;
    if (enough_points)
    {
        for (int i=0; i<num_of_segments; ++i)
        {
            if (segments_x.at(i).back() - segments_x.at(i).front() > 0)
                segment_directions_x.push_back(1);
            else if (segments_x.at(i).back() - segments_x.at(i).front() < 0)
                segment_directions_x.push_back(-1);
            else
                segment_directions_x.push_back(0);
        }
        for (int i=0; i<num_of_segments; ++i)
        {
            if (segments_y.at(i).back() - segments_y.at(i).front() > 0)
                segment_directions_y.push_back(-1);
            else if (segments_y.at(i).back() - segments_y.at(i).front() < 0)
                segment_directions_y.push_back(1);
            else
                segment_directions_y.push_back(0);
        }
    }

    int x_dir_changes = 0;
    int y_dir_changes = 0;
    if (enough_points)
    {
        for (int i=0; i<segment_directions_x.size() - 1; ++i)
        {
            if (segment_directions_x.at(i) != segment_directions_x.at(i + 1))
                x_dir_changes += 1;
        }
        for (int i=0; i<segment_directions_y.size() - 1; ++i)
        {
            if (segment_directions_y.at(i) != segment_directions_y.at(i + 1))
                y_dir_changes += 1;
        }
    }

    std::string shape;
    if (!enough_points)
        shape = "Dot";

    else if (segment_directions_x.front() == 1
             && segment_directions_x.back() == -1
             && segment_directions_y.back() == 1
             && segments_delta_XY_ratio.front() >= 1.5
             && x_dir_changes >= 3)
        shape = "Double Horizontal-Vertical Stroke (Hooked)";

    else if (delta_x_y_ratio >= 2.5
             && delta_x >= 50
             && x_direction == -1)
        shape = "Horizontal Stroke\n(Backwards!)";

    else if (delta_x_y_ratio >= 2.5
             && delta_x >= 50
             && segment_directions_x.back() == -1)
        shape = "Horizontal Stroke (Hooked)";

    else if (delta_x_y_ratio >= 2.5
             && delta_x >= 50)
        shape = "Horizontal Stroke";

    else if (delta_x_y_ratio <= 0.2
             && delta_y >= 50
             && y_direction == 1)
        shape = "Vertical Stroke\n(Backwards!)";

    else if (delta_x_y_ratio <= 0.2
             && delta_y >= 50
             && segment_directions_y.back() == 1
             && segment_directions_x.back() == -1)
        shape = "Vertical Stroke (Hooked)";

    else if (delta_x_y_ratio <= 0.2
             && delta_y >= 50
             && segment_directions_y.back() == 1
             && segment_directions_x.back() == 1)
        shape = "Vertical Stroke (Hooked)\n(Backwards!)";

    else if (delta_x_y_ratio <= 0.2 && delta_y >= 50)
        shape = "Vertical Stroke";

    else if (segment_directions_x.front() == 1
             && segment_directions_x.back() == -1
             && segment_directions_y.back() == 1
             && segments_delta_XY_ratio.front() >= 2.5)
        shape = "Horizontal-Vertical Stroke (Hooked)";

    else if (segment_directions_x.front() == 1
             && segment_directions_x.back() == 1
             && segment_directions_y.back() == 1
             && segments_delta_XY_ratio.front() >= 2.5)
        shape = "Horizontal-Vertical Stroke (Hooked)\n(Backwards!)";

    else if (segment_directions_x.front() == 1
             && segment_directions_y.back() == -1
             && segments_delta_XY_ratio.front() >= 2.5
             && segments_delta_XY_ratio.back() <= 0.4)
        shape = "Horizontal-Vertical Stroke";

    else if (x_direction == 1
             && y_direction == -1
             && segment_directions_y.front() == -1
             && segment_directions_y.back() == 1
             && segments_delta_XY_ratio.front() <= 0.3
             && segments_y_delta.back() >= 25)
        shape = "Vertical-Horizontal Stroke (Hooked)";

    else if (segment_directions_y.front() == -1
             && segment_directions_x.back() == 1
             && segments_delta_XY_ratio.front() <= 0.4
             && segments_delta_XY_ratio.back() >= 2.5)
        shape = "Vertical-Horizontal Stroke";

    else if (segment_directions_x.front() == 1
             && segment_directions_x.back() == -1
             && segment_directions_y.back() == -1
             && segments_delta_XY_ratio.front() >= 2.5
             && segments_delta_XY_ratio.back() >= 0.5
             && segments_delta_XY_ratio.back() <= 1.5)
        shape = "Horizontal-Falling Right Stroke";

    else if (x_direction == -1
             && y_direction == -1
             && delta_x >= 50
             && delta_y >= 50
             && segment_directions_y.back() == 1)
        shape = "Falling Left Stroke (Hooked)";

    else if (x_direction == -1
             && y_direction == -1
             && delta_x >= 50
             && delta_y >= 50)
        shape = "Falling Left Stroke";

    else if (x_direction == 1
             && y_direction == -1
             && segment_directions_y.back() == 1
             && segment_directions_x.back() == -1
             && delta_x >= 50
             && delta_y >= 50)
        shape = "Falling Right Stroke (Hooked Left)";

    else if (x_direction == 1
             && y_direction == -1
             && segment_directions_y.back() == 1
             && segment_directions_x.back() == 1
             && delta_x >= 50
             && delta_y >= 50)
        shape = "Falling Right Stroke (Hooked Right)";

    else if (x_direction == 1
             && y_direction == -1
             && delta_x >= 50
             && delta_y >= 50)
        shape = "Falling Right Stroke";

    else if (x_direction == -1
             && y_direction == 1
             && delta_x >= 50
             && delta_y >= 50)
        shape = "Rising Left Stroke";

    else if (x_direction == 1
             && y_direction == 1
             && delta_x >= 50
             && delta_y >= 50)
        shape = "Rising Right Stroke";

    else
        shape = "Dot";

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
    std::cout << "Horizontal Direction: " << x_direction << std::endl;
    std::cout << "Vertical Direction: " << y_direction << std::endl;
    std::cout << "X Direction Changes: " << x_dir_changes << std::endl;
    std::cout << "Y Direction Changes: " << y_dir_changes << std::endl;

    for (int i=0; i<segments_x.size(); ++i)
    {
        std::cout << "X Segment " << i << ": " <<
                     segments_x.at(i).front() << " " <<
                     segments_x.at(i).back() << "\tDirection: " <<
                     segment_directions_x.at(i) <<
                     std::endl;
    }
    for (int i=0; i<segments_y.size(); ++i)
    {
        std::cout << "Y Segment " << i << ": " <<
                     segments_y.at(i).front() << " " <<
                     segments_y.at(i).back() << "\tDirection: " <<
                     segment_directions_y.at(i) <<
                     std::endl;
    }

    ui->strokeLabel->setText(QString::fromStdString(shape));
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    int thickness = 5;
    int num_of_points = mouse_drag_points_x->size();

    QPainter painter(this);
    painter.setPen(Qt::red);
    for (int i=0; i<num_of_points; ++i)
    {
        for (int tx=-thickness; tx<=thickness; ++tx)
        {
            for (int ty=-thickness; ty<=thickness; ++ty)
            {
                QPoint p(mouse_drag_points_x->at(i)+ tx, mouse_drag_points_y->at(i) + ty);
                painter.drawPoint(p);
            }
        }
    }
    painter.end();
}
