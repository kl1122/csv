#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCharts/QChartView>//显示图表
#include <QtCharts/QLineSeries>//线系列
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("pluse脉冲解析");
    setFixedSize(940,740);
    connect(ui->btn_read, &QPushButton::clicked, this ,[=](){//打开文件

        QT_CHARTS_USE_NAMESPACE

        QString FileName = QFileDialog::getOpenFileName(this,"打开文件","C:\\");
        if(FileName.isEmpty() == true)
        {
            QMessageBox::warning(this,"warning","open file failed!");
            return;
        }
        ui->lineEdit->setText(FileName);


        QList<QLineSeries *> m_series;
        QLineSeries *series = new QLineSeries();
        m_series.append(series);


        //设置线条名称
        series->setName(QString("csv解析图谱"));

        //设置线条颜色，如果不设置会给默认颜色
        series->setColor(QColor(255,0,255));
        //设置是否线条可视
        series->setVisible(true);

        //点标签是否可视
        series->setPointLabelsVisible(true);

        //点标签颜色
        series->setPointLabelsColor(QColor(255,255,255));

        //点标签字体
        series->setPointLabelsFont(QFont("微软雅黑"));

        //设置点标签显示格式
        series->setPointLabelsFormat(" ");

        //是否切割边缘点标签，默认为true
        series->setPointLabelsClipping(false);

        QFile file(FileName);
        if(file.open(QIODevice::ReadOnly))
        {
           QTextStream stream(&file);
           while(!file.atEnd())
           {
               QStringList list = stream.readAll().split('\n');
               QListIterator<QString> li(list);
               QString str = list.join(",");

               QString str1 = str.simplified();   //去除字符串前后的空白符，并用“ ”分开
               str1.remove(QRegExp("\\s"));       //去除所有字符串中出现的“ ”
               QString str2 = str1.section(",",0,1023);   //只取脉冲信号列表的前1024项
               ui->textEdit->setText(str2);


               float buf;
               int x = 1;
               series->clear();
               while(li.hasNext())
               {

                   buf=li.next().toFloat();
                   if(x <= 1024)
                   {
                       series->append(x,buf * 32768);
                       x++;
                   }
                   else
                       break;
                }
           }
        }

        QChart *chart = new QChart();
        chart->setTheme(QChart::ChartThemeBlueCerulean);//设置系统主题
        chart->setAnimationOptions(QChart::AllAnimations);//设置启用或禁用动画
        chart->setLocalizeNumbers(true);//数字是否本地化
        chart->addSeries(series);//添加系列到QChart上

        chart->createDefaultAxes();//创建默认轴
        chart->setTitle("脉冲图像");//设置标题
        chart->setTitleBrush(QBrush(QColor(25,170,255)));//设置标题Brush
        chart->setTitleFont(QFont("微软雅黑"));//设置标题字体

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);//底部对齐
        chart->legend()->setBackgroundVisible(true);//设置背景是否可视
        chart->legend()->setLabelColor(QColor(255,128,255));//设置标签颜色
        chart->legend()->setVisible(true);//设置是否可视
        chart->legend()->setBorderColor(QColor(255,255,170,185));//设置边框颜色
        QFont font = chart->legend()->font();
        font.setItalic(!font.italic());
        chart->legend()->setFont(font);//设置字体为斜体
        font.setPointSizeF(12);
        chart->legend()->setFont(font);//设置字体大小
        chart->legend()->setFont(QFont("微软雅黑"));//设置字体类型

        QChartView *chartView = new QChartView(chart);
        QGridLayout *baselayout = new QGridLayout();
        
        baselayout->addWidget(chartView,1,0);
        chartView->setChart(chart);
        ui->chart_view->setLayout(baselayout);

        connect(ui->btn_save,&QPushButton::clicked,this,[=](){//保存文件
            QFile sfile("pluse.txt");
            if(sfile.open(QFile::WriteOnly))
            {
                QTextStream out(&sfile);
                //out<<ui->textEdit->setText()
                out<<ui->textEdit->toPlainText()<<endl<<QObject::tr("");
                QMessageBox::warning(this,"保存中...","文件保存成功!");
            }
        });

        file.close();

    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

