/**
* @file     widget.h
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @class    Widget
* @brief    this class is used to show the chart for all sensors.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @class    SignalChart,LegPositionChart,LegChart
* @brief    there are three classes for different signal types,different position and different leg.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @author   Flame
* @date     03.05.2019
*/

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include "ImuChart.hpp"
#include "ImageWidget.h"
#include "FlameList.hpp"
#include <algorithm>
#include "Dataframe.hpp"

class ImuProcesserWidget;
typedef unsigned int uint;
typedef MarkerChart<ImuProcesserWidget> ImuChart;
typedef ImuChart* ImuChartPoint;
typedef std::vector<ImuChartPoint> ImuChartList;

namespace Ui {
class Widget;
}

class ImuProcesserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImuProcesserWidget(QWidget *parent = nullptr,bool ImageWindow = false);
    bool ChangeRange(int position);
    ~ImuProcesserWidget();
    void closeEvent(QCloseEvent *event);
    bool ChangeImage(int value);
    bool ChangeImageLine(int value);
    // draw & delete the marker line on the last graph
    void drawMarkerLine(int id,int position);
    bool delMarkerLine(int id, int index, int position);
    void replot();
protected:
    ImageWidget imagewidget;
    DataFrame<double> dataframe;
    ImuChartList allChart;
    void timerEvent(QTimerEvent*event);
private slots:
    void on_pushButton_clicked();    
    void on_checkBox_stateChanged(int arg1);    
    void on_PlaypushButton_clicked();
    void on_RedcheckBox_stateChanged(int show);
    void on_OutputpushButton_clicked();
    void on_axisXRangespinBox_editingFinished();
    void on_FilenamelineEdit_editingFinished();
    void on_ImagPositionhorizontalSlider_valueChanged(int value);
    void on_SelectpushButton_clicked();
    void on_pushButton_2_clicked();    
    void on_marklabel_editingFinished();
    void on_comboBox_activated(int index);
    void on_WindowPositionhorizontalSlider_valueChanged(int value);
    void on_ZoomverticalSlider_valueChanged(int value);
    void on_preview_clicked();

private:
    bool isRT;    
    int timeid;
    double zoom;
    bool isplay;
    bool isSaved;
    bool showed;
    unsigned int activityCount;
    unsigned long long int length;

    int RangePosition;
    int ImagePosition;
    flame::MarkList<unsigned long long ,int> marklist;

    bool showImage;
    bool haveObtainData;
    unsigned int axisXMax;    

    QString rootDir;
    QString ImageDir;    
    QString ImageLogFile;
    int firstImageNumber;

    QImage img;
    QImageReader reader;

    Ui::Widget *ui;

    QWidget * showTableWindows;
    QLabel * showTableLabel;
    QGridLayout * showTableLayout;    
    QVector<QString> activityStr = {"downstairs","upstairs","downslope","upslope","sit","stand","walk"};
    QString invalidStr = "none";

    std::vector<std::string> tableStr;
    unsigned long long int tableCount = 0;
};

#endif // WIDGET_H
