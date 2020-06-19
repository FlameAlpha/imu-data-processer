/**
* @file     ImuChart.h
* @class    ImuChart
* @brief    this file includes class(ImuChart) for show data from csv file.
* @variable ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*           QChart* chart;  // used to show all lines include curves and vertical lines
*           QVector<int> positions; // vector for psoition of vertical lineseries
*           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @author   Flame
* @date     03.05.2019
*/

#ifndef IMUCHART_H
#define IMUCHART_H

#include "QCustomplot.h"
#include <iostream>
#include <cmath>

template <typename Window>
class MarkerChart : public QCustomPlot {
public:
    MarkerChart(Window * parent,unsigned long long int chartID,QString title="",double rx=500, double uy=100, double lx=0, double dy=-100, bool axisVisible=true, bool legendVisible=false) {
        init(parent,chartID,title,rx,uy,lx,dy,axisVisible,legendVisible);
    }

    MarkerChart(QString title="",double rx=500, double uy=100, double lx=0, double dy=-100, bool axisVisible=true, bool legendVisible=false) {
        init(nullptr,0,title,rx,uy,lx,dy,axisVisible,legendVisible);
    }

    ~MarkerChart(){
        delete titleItem;
        delete whiteItem;
    }

    void init(Window * parent,unsigned long long int chartID,QString title="",double rx=500, double uy=100, double lx=0, double dy=-100, bool axisVisible=true, bool legendVisible=false) {
        this->parent = parent;
        this->chartID = chartID;
        this->title = title;

        // set user interface parameter
        this->setInteractions(QCP::iRangeZoom); //QCP::iRangeZoom | QCP::iRangeDrag | QCP::iSelectAxes| QCP::iSelectPlottables

        // set range of axies
        setRange(rx,uy,lx,dy);

        // set label of axies
        this->xAxis->setLabel("axisX");
        this->yAxis->setLabel("axisY");        

        // set axies is visible
        this->xAxis->setVisible(axisVisible);
        this->yAxis->setVisible(axisVisible);

        // set title string
        this->plotLayout()->insertRow(0);
        this->plotLayout()->insertRow(1);
        titleItem = new QCPTextElement(this, title, QFont("sans", 10, QFont::Bold));
        titleItem->setTextFlags(Qt::AlignBottom|Qt::AlignHCenter);
        whiteItem = new QCPTextElement(this, title, QFont("", 1));
        this->plotLayout()->addElement(0, 0, whiteItem);
        this->plotLayout()->addElement(1, 0, titleItem);

        // set style of legend
        QFont legendFont = font();
        legendFont.setPointSize(10);
        this->legend->setFont(legendFont);
        this->legend->setSelectedFont(legendFont);
        this->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
        this->legend->setVisible(legendVisible);
        this->addGraph();
        this->drawImageLine(0);
        this->addGraph();
        QVector<double> data;
        for (int i=0;i<rx;i++) {
            data << 50*std::sin(i/(2*3.1415926*2.5));
        }
        this->drawSerie(data);
    }

    // set the title of graph
    void setTitle(QString title){
        this->title = title;
        titleItem->setText(title);
    }

    // set the id of graph
    void setID(unsigned int id){
        this->chartID = id;
    }

    // change the range of x axis
    void setAxisXRange(double rx=500, double lx=0){
        setRange(rx,this->uy,lx,this->dy);
    }

    // change the range of y axis
    void setAxisYRange(double uy=100, double dy=-100){
        setRange(this->rx,uy,this->lx,dy);
    }

    // zoom the x axis
    void ZoomAxisX(double zoomx){
        zoomx = zoomx >= 1 ? zoomx : 1;
        zoom(1/zoomx,1);
    }

    // zoom the y axis
    void ZoomAxisY(double zoomy){
        zoomy = zoomy >= 1 ? zoomy : 1;
        zoom(1,1/zoomy);
    }

    // set range of all axies
    void setRange(double rx=500, double uy=100, double lx=0, double dy=-100, bool autorange=false){
        this->rx = rx;
        this->lx = lx;
        this->uy = uy;
        this->dy = dy;

        if(this->uy < 0.1)
            this->uy = 0.1;

        if(this->dy > -0.1)
            this->dy = -0.1;

        this->xAxis->setRange(lx, rx);
        this->yAxis->setRange(dy, uy);

        this->axisRect()->setupFullAxesBox();
        //this->replot();
        if(autorange)
            this->autoRange();
    }

    // the width of x axis
    double xAxisRange(){
        return rx - lx;
    }

    // the height of y axis
    double yAxisRange(){
        return uy - dy;
    }

    // set the data of serie
    void setSerieData(QVector<double> y){
        QVector<double> x;
        for(int i=0;i<y.size();i++)
            x << i;
        this->graph(dataLineIndex)->setData(x,y);
        //this->replot();
    }

    // marker visible setting
    void setMarkerVisible(bool visible){
        for (int i=2;i<this->graphCount();i++) {
            this->graph(i)->setVisible(visible);
        }
        //this->replot();
    }

    // change the position of image line
    void changeImageLine(int position){
        QVector<double> x;
        QVector<double> y;
        x << position << position;
        y << dy << uy;
        imagePosition = position;
        this->graph(imageLineIndex)->setData(x, y);
        //this->replot();
    }

    // get the position of marker
    QVector<int> markerPosition(){
        return MarkerPosition;
    }

    // get the position from the index
    int getPos(int index){
        return MarkerPosition[index];
    }

    // return whether or not there is marker on this position
    bool isContain(int position){
        return MarkerPosition.indexOf(position)!=-1?true:false;
    }

    // draw & delete the marker line on the last graph
    void drawMarkerLine(int id,int position){
        if(id!=this->chartID){
            drawMarkerLine(position);
            MarkerPosition.push_back(position);
        }
    }

    bool delMarkerLine(int id,int index,int position){
        if(id!=this->chartID){
            if(this->removeGraph(this->graph(index))){
                //this->replot();
                int iter = MarkerPosition.indexOf(position);
                MarkerPosition.remove(iter);
                return true;
            }else return false;
        }else return true;
    }

    // clear all vertical line
    void clearLineList(){
        MarkerPosition.clear();
        while (MarkerIndexBegin < this->graphCount()) {
            this->removeGraph(this->graph(MarkerIndexBegin));
        }
        //this->replot();
    }

    // get chart id
    int getID(){
        return chartID;
    }

    std::string getTitle(){
        return title.toStdString();
    }

private:
    int abs(int value){
        return value < 0  ? -value : value;
    }

    // auto adapt the changing of axis' range for all lines
    void autoRange(bool Image=true,bool Marker=true){
        if(Marker)
            for (int i=MarkerIndexBegin;i<this->graphCount();i++) {
                autoRangeChangeVerticalLine(i);
            }
        if(Image)
            autoRangeChangeVerticalLine(imageLineIndex);
    }

    // auto adapt the changing of axis' range for vertical line
    void autoRangeChangeVerticalLine(int graphIndex=1){
        if(graphIndex>=this->graphCount()-1) return;
        auto data = this->graph(graphIndex)->data();
        if(data->size() == 2){
            int position = int(data->begin()->key);
            QVector<double> x;
            QVector<double> y;
            x << position << position;
            y << dy << uy;
            this->graph(graphIndex)->setData(x, y);
            //this->replot();
        }
    }

    void zoom(double zoomx=1,double zoomy=1){
        this->xAxis->setRange(lx*zoomx, rx*zoomx);
        this->yAxis->setRange(dy*zoomy, uy*zoomy);
        this->axisRect()->setupFullAxesBox();
        //this->replot();
    }
    // draw vertical line on "graphIndex" graph
    void drawVerticalLine(int graphIndex, int position, QPen pen){
        QVector<double> x;
        QVector<double> y;        
        x << position << position;
        y << dy << uy;
        this->graph(graphIndex)->setData(x, y);
        this->graph(graphIndex)->setPen(pen);
        this->graph(graphIndex)->setLineStyle(QCPGraph::LineStyle(1));
        this->graph(graphIndex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
        //this->replot();
    }

    // draw vertical line on the last graph
    void drawVerticalLine(int position, QPen pen){
        this->addGraph();
        drawVerticalLine(this->graphCount()-1,position,pen);
    }

    // draw the marker line on the last graph
    void drawMarkerLine(int position){
        MarkerPosition.push_back(position);
        drawVerticalLine(position,QPen(Qt::red,3,Qt::SolidLine));
    }

    // draw the vertical line for image time frame
    void drawImageLine(int position, int width=3){
        imagePosition = position;
        drawVerticalLine(imageLineIndex,position,QPen(QColor(0,238,238),width,Qt::DashLine));
    }

    // draw the serie for data
    void drawSerie(QVector<double> y, int width=3){
        QVector<double> x;
        for (int i=0;i<y.size();i++) {
            x << i;
        }
        this->graph(dataLineIndex)->setData(x, y);
        this->graph(dataLineIndex)->setPen(QPen(QColor(44,159,255),width));
        this->graph(dataLineIndex)->setLineStyle(QCPGraph::LineStyle(1));
        this->graph(dataLineIndex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
        //this->replot();
    }

    // delete marker vertical line
    bool delMarkerLine(int position){
        for (int i=MarkerIndexBegin;i<this->graphCount();i++) {
            auto data = this->graph(i)->data();
            if(data->size() == 2 && abs(int(data->begin()->key)-position)<=2){
                this->removeGraph(this->graph(i));
                //this->replot();
                int iter = MarkerPosition.indexOf(int(data->begin()->key));
                MarkerPosition.remove(iter);
                if(parent!=nullptr){}
                    this->parent->delMarkerLine(this->chartID,i,int(data->begin()->key));
                return true;
            }
        }
        return false;
    }

    void resizeEvent(QResizeEvent *event){
        event->accept();
        this->QCustomPlot::resizeEvent(event);
    }

    // delete the vertical line after double clicking
    void mouseDoubleClickEvent(QMouseEvent *event){
        event->accept();
        int position = int(this->xAxis->pixelToCoord(event->pos().x()));
        if(!delMarkerLine(position)){
            drawMarkerLine(position);
            if(parent!=nullptr){}
                this->parent->drawMarkerLine(this->chartID,position);
        }
    }
private:
    double rx = 0;
    double lx = 0;
    double uy = 0;
    double dy = 0;
    int imageLineIndex = 0;
    int dataLineIndex = 1;
    int MarkerIndexBegin = 2;
    int imagePosition = 0;
    unsigned long long int chartID = 0;
    QString title="";
    QCPTextElement *titleItem=nullptr;
    QCPTextElement *whiteItem=nullptr;
    Window * parent = nullptr;
    QVector<int> MarkerPosition{};
};

#endif //IMUCHART_H
