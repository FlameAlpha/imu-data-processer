#include "ImuProcesserWidget.h"
#include "ui_ImuProcesserWidget.h"

void initTableStrList(std::vector<std::string> & tableStr){
    std::vector<std::string> Leg = { "RT","LT" };
    std::vector<std::string> Legposition = { "Foot","Thigh" };
    std::vector<std::string> columns = {  "GlobalAccX", "GlobalAccY", "GlobalAccZ",
                                         "CourseRate", "Roll", "Pitch"};
    for (const auto & legtemp: Leg)
        for (const auto & legpositiontemp : Legposition) {
            for (const auto & column : columns) {
                tableStr.push_back(legpositiontemp+"_"+ column +"_"+ legtemp);
            }
        }
}

ImuProcesserWidget::ImuProcesserWidget(QWidget *parent, bool ImageWindow) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    rootDir = "C:/";
    showed = true;
    isRT = true;
    axisXMax = 500;
    length = 1000;
    isplay = false;
    isSaved = false;
    activityCount = uint(activityStr.size());
    haveObtainData = false;
    showImage = ImageWindow;

    marklist.append(0,-1);
    marklist.append(length,-1);
    marklist.initendPos();    

    reader.setAutoTransform(true);

    this->ImagePosition = 0;
    this->RangePosition = 0;

    ui->WindowPositionhorizontalSlider->setEnabled(false);
    ui->WindowPositionhorizontalSlider->setValue(0);
    ui->WindowPositionhorizontalSlider->setMinimum(0);
    ui->WindowPositionhorizontalSlider->setMaximum(1000);
    ui->WindowPositionhorizontalSlider->setEnabled(true);

    // init table str list
    initTableStrList(tableStr);
    if(tableStr.size() > 24)
        throw (std::out_of_range("the num of table is out of range!"));

    for(const auto & tablestr : tableStr){
        allChart.emplace_back(new ImuChart(this,tableCount++));
        allChart.back()->setTitle(QString::fromStdString(tablestr));
    }

    setWindowState(Qt::WindowMaximized);
    this->setWindowTitle("Imu Data Processer");

    unsigned long long int max_chart = 12;

    if(allChart.size() < max_chart){
        max_chart = allChart.size();
    }

    for (unsigned long long int i = 0; i < max_chart; ++i) {
        ui->gridLayoutCharts->addWidget(allChart[i],i%4,i/4);
    }

    this->ui->comboBox->clear();
    for (auto activity:activityStr) {
        this->ui->comboBox->addItem(activity);
    }

    this->ui->comboBox->addItem(invalidStr);
    ui->comboBox->setEnabled(false);
    ui->comboBox->setCurrentIndex(activityCount);
    ui->comboBox->setEnabled(true);

    showTableWindows = new QWidget;
    showTableLabel = new QLabel(showTableWindows);
    showTableLayout = new QGridLayout(showTableWindows);
    showTableLayout->addWidget(showTableLabel);
    showTableWindows->setLayout(showTableLayout);
    showTableLayout->setSizeConstraint(QLayout::SetFixedSize);
    showTableLabel->setFont(QFont("Microsoft YaHei", 10));    
}

ImuProcesserWidget::~ImuProcesserWidget()
{       
    delete showTableWindows;
    delete ui;
}

void ImuProcesserWidget::on_pushButton_clicked()
{    
    // read data
    std::string Filename = ui->FilenamelineEdit->text().toStdString();
    std::cout<<"Filename : "<<rootDir.toStdString()+"/"+Filename+".csv"<<std::endl;
    if(!dataframe.read_csv(rootDir.toStdString()+"/"+Filename+".csv",',')){
        QMessageBox msgBox;
        msgBox.setStyleSheet("QPushButton {"
                            " font: bold 20px;"
                            " min-width: 4.5em;"
                            " min-height: 1.2em;"
                            "}"
                            "QLabel{"
                            "min-width: 300px;"
                            "min-height: 30px;"
                            "max-height: 30px;"
                            "font-size:22px;"
                            "}");
        msgBox.setText("file is invalid !");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
        return;
    }

    length = dataframe.row_num();
    qDebug()<<"Read over! ";
    if(!dataframe.empty())
        qDebug()<<"Data is vaild ";
    else {
        qDebug()<<"Data is invaild ";
        return;
    }

    // clear marker data    
    marklist.clear();
    marklist.append(0,-1);
    marklist.append(length,-1);
    marklist.initendPos();    

    // read image info
    ImageDir = rootDir+"/"+ui->FilenamelineEdit->text();
    ImageLogFile = ImageDir+"/"+"log.txt";
    std::ifstream Reader(ImageLogFile.toStdString().data());
    std::string strLineContext;
    getline(Reader, strLineContext);
    QString FirstImage = QString::fromStdString(strLineContext.substr(strLineContext.find_first_of("_", 0)+1, strLineContext.size()));
    std::cout<<"First Image number :"<<FirstImage.toStdString()<<std::endl;
    firstImageNumber = FirstImage.toInt();

    // change axis range
    this->RangePosition = 0;
    ChangeRange(0);

    // change image & imageLine
    this->ImagePosition = RangePosition+ui->ImagPositionhorizontalSlider->value();
    if(showImage)
        imagewidget.show();
    ChangeImage(this->ImagePosition);

    // change the state of all sliders
    ui->WindowPositionhorizontalSlider->setEnabled(false);
    ui->WindowPositionhorizontalSlider->setValue(0);
    ui->WindowPositionhorizontalSlider->setMinimum(0);
    ui->WindowPositionhorizontalSlider->setMaximum(int(length-axisXMax));
    ui->WindowPositionhorizontalSlider->setEnabled(true);
    ui->ImagPositionhorizontalSlider->setEnabled(false);
    ui->ImagPositionhorizontalSlider->setValue(0);
    ui->ImagPositionhorizontalSlider->setMinimum(0);
    ui->ImagPositionhorizontalSlider->setMaximum(500);
    ui->ImagPositionhorizontalSlider->setEnabled(true);

    // draw the graph by data
    unsigned int i = 0;
    auto marker = dataframe["marker"];
    std::cout<<"loss package : "<<std::count(marker.begin(),marker.end(),0)<<std::endl;

    //state setting
    this->haveObtainData = true;
    this->isSaved = false;

    for(auto chart : allChart){
        chart->clearLineList();
        auto minY = std::min_element(dataframe[chart->getTitle()].begin(),dataframe[chart->getTitle()].end()-1);
        auto maxY = std::max_element(dataframe[chart->getTitle()].begin(),dataframe[chart->getTitle()].end()-1);
        chart->setRange(500,*maxY+fabs((*maxY-*minY)/10.0),0  ,*minY-fabs((*maxY-*minY)/10.0),true);
        // std::cout << chart->getTitle() << ' ' << *maxY+fabs((*maxY-*minY)/5.0) << ',' << *minY-fabs((*maxY-*minY)/5.0) <<std::endl;
        chart->setSerieData(QVector<double>::fromStdVector(dataframe[chart->getTitle()].toStdVector()));
        i++;
    }

    this->ChangeImage(this->ImagePosition);
    this->replot();
    std::cout<<"Init over!"<<std::endl;
}

void ImuProcesserWidget::on_checkBox_stateChanged(int arg1)
{
    isRT=bool(arg1);

    if(allChart.size()>24)
        return;

    for (unsigned int i = 0; i < 12; i++) {
        auto & accangle = allChart[i];        
        if(!isRT)
            ui->gridLayoutCharts->removeWidget(accangle);
        else
            ui->gridLayoutCharts->addWidget(accangle,i%4,i/4);
        accangle->setVisible(isRT);
    }

    for (unsigned int i = 12; i < 24; i++) {
        auto & accangle = allChart[i];
        if(isRT)
            ui->gridLayoutCharts->removeWidget(accangle);
        else
            ui->gridLayoutCharts->addWidget(accangle,i%4,i/4);
        accangle->setVisible(!isRT);
    }

    this->replot();
}

void ImuProcesserWidget::on_PlaypushButton_clicked(){
    isplay = !isplay;
    if(isplay)
        timeid = startTimer(10);
    else
        this->killTimer(timeid);
    ui->WindowPositionhorizontalSlider->setDisabled(isplay);
}

void ImuProcesserWidget::timerEvent(QTimerEvent *event){
    if(event->timerId()==timeid)
    {
        RangePosition+=10*ui->playspeeddoubleSpinBox->value();
        ImagePosition = RangePosition+ui->ImagPositionhorizontalSlider->value();
        if(uint(RangePosition)>=length-axisXMax)
        {
            RangePosition = 0;
            ImagePosition = RangePosition+ui->ImagPositionhorizontalSlider->value();
            this->killTimer(timeid);
            isplay = !isplay;            
            ui->PlaypushButton->setChecked(false);
            ui->WindowPositionhorizontalSlider->setDisabled(isplay);
        }                
        ChangeImageLine(ImagePosition);
        ChangeRange(RangePosition);
        ChangeImage(ImagePosition);
        this->replot();
        ui->WindowPositionhorizontalSlider->setValue(RangePosition);
    }
}

bool ImuProcesserWidget::ChangeRange(int position)
{
    for(const auto & accangle:allChart)
    {
        accangle->setAxisXRange(uint(position)+axisXMax,position);
    }
    return true;
}

void ImuProcesserWidget::on_RedcheckBox_stateChanged(int show)
{
    showed = show;
    for(const auto & accangle:allChart)
    {
        accangle->setMarkerVisible(showed);
    }
    this->replot();
}

void ImuProcesserWidget::drawMarkerLine(int id, int position)
{
    for(const auto & accangle:allChart)
    {
        accangle->drawMarkerLine(id,position);
    }
    this->replot();
    marklist.isChangeTag(position);
    marklist.insertPos(position,marklist.getTag());
    marklist.isChangeTag(ImagePosition);
    this->isSaved = false;
}

bool ImuProcesserWidget::delMarkerLine(int id,int index,int position)
{    
    this->setEnabled(false);
    for(const auto & accangle:allChart)
    {
       if(!accangle->delMarkerLine(id,index,position))
           return false;
    }

    this->replot();        
    this->setEnabled(true);
    marklist.delPos(position);
    if(marklist.isChangeTag(ImagePosition)){
        int label = marklist.getTag();
        ui->comboBox->setCurrentIndex(label >= 0 ? label : 7);
        ui->marklabel->setText(QString::number(label));
    }
    this->isSaved = false;
    return true;
}

void ImuProcesserWidget::replot()
{
    for(const auto & accangle:allChart)
    {
       accangle->replot();
    }
}

void ImuProcesserWidget::on_OutputpushButton_clicked()
{
    if(marklist.size()<=2 || !haveObtainData){
//        std::cout<<"marklist is empty ! "<<std::endl;
        QMessageBox msgBox;
        msgBox.setStyleSheet("QPushButton {"
    //                        "background-color:#89AFDE;"
    //                        " border-style: outset;"
    //                        " border-width: 2px;"
    //                        " border-radius: 10px;"
    //                        " border-color: beige;"
                            " font: bold 20px;"
                            " min-width: 4.5em;"
                            " min-height: 1.2em;"
                            "}"
                            "QLabel{"
                            "min-width: 300px;"
                            "min-height: 30px;"
                            "max-height: 30px;"
                            "font-size:22px;"
    //                        "background-color:#f0f0f0;"
                            "}");
        msgBox.setText("marklist is empty !");
//        msgBox.setInformativeText("marklist is empty !");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.exec();
        return;
    }
    else{
        this->isSaved = true;
        QString address = rootDir+"/"+rootDir.section('/', -1)+"_Process";
        qDebug()<<"address : "<<address;
        QDir dir;
        if(!dir.exists(address))
        {
                std::cout<<"The path does not exist ! "<<std::endl;
                dir.mkpath(address);
        }
        QString Allfilename = address+"/"+ui->lineEdit->text()+".csv";

        std::vector<double> activity_vector;
        for (unsigned int var = 0; var < length; var++) {
            marklist.isChangeTag(var);
            activity_vector.push_back(marklist.getTag());
        }

        dataframe["activity"] = activity_vector;
        dataframe.to_csv(Allfilename.toStdString(),',');
        {
            QMessageBox msgBox;
            msgBox.setStyleSheet("QPushButton {"
        //                        "background-color:#89AFDE;"
        //                        " border-style: outset;"
        //                        " border-width: 2px;"
        //                        " border-radius: 10px;"
        //                        " border-color: beige;"
                                " font: bold 20px;"
                                " min-width: 4.5em;"
                                " min-height: 1.2em;"
                                "}"
                                "QLabel{"
                                "min-width: 300px;"
                                "min-height: 30px;"
                                "max-height: 30px;"
                                "font-size:22px;"
        //                        "background-color:#f0f0f0;"
                                "}");
            msgBox.setText("Writing is over !");
    //        msgBox.setInformativeText("marklist is empty !");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.exec();
        }
    }
}

void ImuProcesserWidget::on_axisXRangespinBox_editingFinished()
{
    for(const auto & accangle:allChart)
    {
        axisXMax = uint(ui->axisXRangespinBox->value());
        ui->ImagPositionhorizontalSlider->blockSignals(true);
        ui->WindowPositionhorizontalSlider->blockSignals(true);
        ui->ImagPositionhorizontalSlider->setMaximum(int(axisXMax));
        ui->WindowPositionhorizontalSlider->setMaximum(int(length-axisXMax));
        ui->ImagPositionhorizontalSlider->blockSignals(false);
        ui->WindowPositionhorizontalSlider->blockSignals(false);
        accangle->setAxisXRange(uint(RangePosition)+axisXMax,RangePosition);
    }
    this->replot();
}

void ImuProcesserWidget::on_FilenamelineEdit_editingFinished()
{
    ui->lineEdit->setText(ui->FilenamelineEdit->text());
}

void ImuProcesserWidget::closeEvent(QCloseEvent *event)
{      
    if(this->haveObtainData && marklist.size()>2 && !this->isSaved){
        QMessageBox msgBox;
        msgBox.setStyleSheet("QPushButton {"
        //                        "background-color:#89AFDE;"
        //                        " border-style: outset;"
        //                        " border-width: 2px;"
        //                        " border-radius: 10px;"
        //                        " border-color: beige;"
                            " font: bold 20px;"
                            " min-width: 4.5em;"
                            " min-height: 1.2em;"
                            "}"
                            "QLabel{"
                            "min-width: 400px;"
                            "min-height: 30px; "
                            "font-size:20px;"
        //                        "background-color:#f0f0f0;"
                            "}");
        msgBox.setText("<font size='4' color='black'>The project has been modified.</font>");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
           case QMessageBox::Save:
               // Save was clicked
               this->on_OutputpushButton_clicked();
               imagewidget.close();
               showTableWindows->close();
               event->accept();
               break;
           case QMessageBox::Discard:
               // Don't Save was clicked
               imagewidget.close();
               showTableWindows->close();
               event->accept();
               break;
           case QMessageBox::Cancel:
               // Cancel was clicked
               event->ignore();
               break;
           default:
               // should never be reached
               event->ignore();
               break;
         }
    }else {
        imagewidget.close();
        showTableWindows->close();
        event->accept();
    }
}

bool ImuProcesserWidget::ChangeImageLine(int value)
{
    for(const auto & accangle:allChart)
    {
        accangle->changeImageLine(value);
    }

    if(marklist.isChangeTag(value)){
        int label = marklist.getTag();
        ui->comboBox->setCurrentIndex(label >= 0 ? label : 7);
        ui->marklabel->setText(QString::number(label));
    }
    return true;
}

void ImuProcesserWidget::on_ImagPositionhorizontalSlider_valueChanged(int value)
{
    if(!isplay){
        this->ImagePosition = value+RangePosition;
        ChangeImage(this->ImagePosition);
        ChangeImageLine(this->ImagePosition);
        this->replot();
    }
}

bool ImuProcesserWidget::ChangeImage(int value)
{
    if(haveObtainData && showImage){
        int temp;
        for(temp = 0;temp<100;temp++){
            reader.setFileName(ImageDir+"/"+QString::number(firstImageNumber+10*value+temp)+".jpg");
            img = reader.read();
            if (!img.isNull()) {
                break;
            }

            reader.setFileName(ImageDir+"/"+QString::number(firstImageNumber+10*value-temp)+".jpg");
            img = reader.read();
            if (!img.isNull()) {
                break;
            }
        }
        if(temp<30){
            imagewidget.draw(img);
            imagewidget.show();
        }else {
            //QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     //tr("Cannot load Image!"));
            std::cout<<"Cannot load Image!"<<std::endl;
            return false;
        }
        return true;
    }else imagewidget.close();
    return false;
}

void ImuProcesserWidget::on_SelectpushButton_clicked()
{
    rootDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "C:/ImuLog/",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    ui->rootDirLabel->setText(rootDir);
}

void ImuProcesserWidget::on_pushButton_2_clicked()
{
    drawMarkerLine(-1,this->ImagePosition);
}

void ImuProcesserWidget::on_marklabel_editingFinished()
{
    int index = ui->marklabel->text().toInt();
    marklist.setCurrentLabel((index < activityCount && index >= -1) ? index : -1);
    ui->comboBox->setCurrentIndex((index < activityCount && index >= 0) ? index: activityCount);
}

void ImuProcesserWidget::on_comboBox_activated(int index)
{
    marklist.setCurrentLabel(index < activityCount ? index : -1);
    ui->marklabel->setText(QString::number(index < activityCount ? index : -1));
}

void ImuProcesserWidget::on_WindowPositionhorizontalSlider_valueChanged(int value)
{
    if(!isplay){
        this->RangePosition = value;
        ImagePosition = RangePosition+ui->ImagPositionhorizontalSlider->value();
        ChangeImageLine(ImagePosition);
        ChangeRange(RangePosition);
        ChangeImage(ImagePosition);
        this->replot();
    }
}

void ImuProcesserWidget::on_ZoomverticalSlider_valueChanged(int value)
{
    zoom = double(value)/5000.0;
    for(auto accangle:allChart){
        accangle->ZoomAxisY(zoom);
    }
    this->replot();
}

void ImuProcesserWidget::on_preview_clicked()
{
//    QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
//                             QString::fromStdString(marklist.toStr()));
    showTableLabel->setText(QString::fromStdString(marklist.toTable()));
    showTableWindows->show();
}
