#include "graph.h"

#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include <QtWidgets/QGraphicsLayout>
#include <QtGui/QMouseEvent>
#include <QtCharts/QValueAxis>

// Sample at 50hz, 1 minute == 3000 samples
#define SIZE 10

Graph::Graph(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      chart(0)
{
    // chart
    chart = new QChart;
    //chartView = new QChartView(chart);
    //chart->setMinimumSize(640, 480);
    chart->legend()->hide();
    series = new QLineSeries;
    chart->addSeries(series);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);

    axisX = new QValueAxis;
    axisX->setRange(0, SIZE);
    axisX->setLabelFormat("%g");
    //axisX->setTitleText("Samples");
    axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    //axisY->setTitleText("Audio level");
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    //chart->setTitle("Data from the microphone");

    //setRenderHint(QPainter::Antialiasing);
    scene()->addItem(chart);

}

void Graph::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    chart->scroll(numDegrees.x(),numDegrees.y());

    //this->scroll(numDegrees.x,numDegrees.y);
    //chart->scroll(numDegrees.x, numDegrees.y);

    qDebug() << numDegrees;

    if (!numPixels.isNull())
    {
       // scrollWithPixels(numPixels);
    }
    else if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        //scrollWithDegrees(numSteps);
    }

    event->accept();
}

void Graph::slot_Value(float v)
{
    if(points.size() > SIZE)
    {
        points.pop_front();
    }

    points.push_back(v);

   // axisX->setRange(0,points.size());

    QVector<QPointF> p;

    int i =0;
    for(auto x : points)
    {
        p.push_back(QPointF(i,x));
        i++;
    }

    series->replace(p);
}


void Graph::resizeEvent(QResizeEvent *event)
{
    if (scene())
    {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         chart->resize(event->size());
        // m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height() - 20);
         //m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height() - 20);
         //foreach (Callout *callout, m_callouts)
           //  callout->updateGeometry();
    }
    QGraphicsView::resizeEvent(event);
}
