#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QSettings>
#include <QVariant>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <QMainWindow>
#include <QString>
#include <iostream>

using namespace std;

const QString WINDOW_TITLE = "Modulation of signal";
const QString X_AXIS_LABEL = "Real is blue, Imaginary is red";
const QString Y_AXIS_LABEL = "A";

const QString PATH ="params.ini";
const QString PARAMS = "Params";

const double PI = acos(-1.0);

const int A_DEFAULT = 100;
const int F_DEFAULT = 10;
const int FD_DEFAULT = 1000;
const int N1_DEFAULT = 0;
const int N2_DEFAULT = 400;
const int DF_DEFAULT = 100;
const int RATE_DEFAULT = 200;
const int MEANDR_DEFAULT = 0;

const int A_MIN = 1;
const int A_MAX = 1000000;
const int F_MIN = 1;
const int FD_MIN = 10;
const int FD_MAX = 1000000;
const int N1_MIN = 0;
const int N1_MAX = 1000000;
const int N2_MIN = 2;
const int N2_MAX = 1000000;
const int DF_MIN = 1;
const int RATE_MIN = 1;
const int MEANDR_MIN = 0;
const int MEANDR_MAX = 1;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    /* my diff: magic string was replaced with const QString */
    this->setWindowTitle(WINDOW_TITLE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* my diff: method for normalize params. posibly will be upgrated and will start show */
/* concrete var that was normalized */
/* also i think it has to show debug in the widget */
void normalize(int &val, const int min, const int max)
{
    if (val < min) {
        val = min;
        qDebug() << "Some value was too low and was normalized" << Qt::endl;
    }
    else if (val > max) {
        val = max;
        qDebug() << "Some value was too high and was normalized" << Qt::endl;
    }
    else ;
}

/* my diff: method for check params and fix it */
void MainWindow::sanitizeParams()
{
    normalize(A, A_MIN, A_MAX);
    normalize(fd, FD_MIN, FD_MAX);
    normalize(f, F_MIN, fd / 2);
    normalize(n1, N1_MIN, N1_MAX);
    normalize(n2, N2_MIN, N2_MAX);
    normalize(df, DF_MIN, fd / 2);
    normalize(rate, RATE_MIN, fd / 2);
    normalize(meandr, MEANDR_MIN, MEANDR_MAX);

    if (n2 <= n1) {
        n2 = n1 + 2;
        qDebug() << "N2 was normalized" << Qt::endl;
    }
    else ;
}

/* my diff: now there is 2 buttons to 2 distinct functions */
void MainWindow::on_pbtime_clicked()
{
    /* my diff: updated debug message */
    qDebug() << "Plotting a graph" << Qt::endl;

    /* my diff: magic string was replaced with const QString */
    QSettings sett(PATH, QSettings::IniFormat);

    /* my diff: debugging */
    qDebug() << "Looking for file: " << sett.fileName();
    qDebug() << "File exists: " << QFile::exists(PATH) << Qt::endl;

    /* my diff: used group */
    sett.beginGroup(PARAMS);
    /* my diff: added default values */
    this->A = sett.value("A", A_DEFAULT).toInt();
    this->f = sett.value("f", F_DEFAULT).toInt();
    this->fd = sett.value("fd", FD_DEFAULT).toInt();
    this->n1 = sett.value("n1", N1_DEFAULT).toInt();
    this->n2 = sett.value("n2", N2_DEFAULT).toInt();
    this->df = sett.value("df", DF_DEFAULT).toInt();
    this->rate = sett.value("rate", RATE_DEFAULT).toInt();
    this->meandr = sett.value("meandr", MEANDR_DEFAULT).toInt();
    sett.endGroup();

    /* my diff: serializing params! */
    sanitizeParams();

    /* my diff: ints i and X was deleted! */
    N = ( n2 - n1 ); /* where N is an amount of the points */

    double minY, maxY; /* vars for limiting the OY axis on the graph */

    /* vectors for plotting the graph */
    /* x is an index on OX axis (can be considered as time */
    /* re is a common-mode component (OY axis, blue) */
    /* im is a quadrature component (OY axis, red) */
    QVector<double> x(N), re(N), im(N);

    /* my diff: fixed strange cycle */
    for (int i = 0; i < N; ++i) {
        int X = n1 + i;
        x[i] = X;
        /* my diff: var "pi" was replaced with const PI, must be tested! */
        re[i] = A*(cos(2*PI*f*X/fd));
        im[i] = A*(sin(2*PI*f*X/fd));
    }

    /* setting min and max y for OY axis */
    /* my diff: two distinct cycles was united */
    minY = re[0], maxY = re[0];
    for (int i = 1; i < N; ++i){
        if (re[i] < minY) minY = re[i];
        else if (re[i] > maxY) maxY = re[i];
        else ;

        if (im[i] < minY) minY = im[i];
        else if (im[i] > maxY) maxY = im[i];
        else ;
    }

    ui -> widget -> xAxis -> setLabel(X_AXIS_LABEL);
    ui -> widget -> xAxis -> setRange(n1, n2);

    ui -> widget -> yAxis -> setLabel(Y_AXIS_LABEL);
    /* my diff: method "rescale axis" was deleted, because we`re setting it manually */
    ui -> widget -> yAxis -> setRange(minY, maxY);

    ui -> widget -> addGraph();
    ui -> widget -> graph(0) -> setData(x, re);
    ui -> widget -> graph(0) -> setPen(QPen(Qt::blue));

    ui -> widget -> addGraph();
    ui -> widget -> graph(1) -> setData(x, im);
    ui -> widget -> graph(1) -> setPen(QPen(Qt::red));

    ui -> widget -> replot();
}

/* TODO: review and fixe whole method */
void MainWindow::on_pbspectrum_clicked()
{
    ;
    /*
    ui -> widget -> clearGraphs();

    qDebug()<<"window FFT";
    i = 0;
    QVector < double > out_fftw_graf(N);
    QVector < double > XX(N);
    fftw_complex in_fftw[N], out_fftw[N];
    fftw_plan plan;/*plan_inv;

    for (X=n1; X<n2; X++){
        in_fftw[X].re = A*(cos(2*PI*f*X/fd));
        in_fftw[X].im = A*(sin(2*PI*f*X/fd));
    }

    //        /* создать оценочный план для прямого БПФ на массиве из N точек
    plan = fftw_plan_dft_1d(N, FFTW_FORWARD, FFTW_ESTIMATE);
    /* вычислить спектр сигнала, заданного массивом in, в соответствии с планом plan
    fftw_one(plan, in_fftw, out_fftw);
    //        /* вычислить сигнала по спектру, заданному массивом out, в соответствии с планом plan_inv
    ////        fftw_one(plan_inv, out, in);

    //        /* удаление ранее созданного плана
    //        fftw_destroy_plan(plan);
    //        fftw_destroy_plan(plan_inv);

    double minYY = out_fftw[0].re, maxYY = out_fftw[0].re;
    for(i=0; i<N; i++){
        out_fftw_graf[i] = (out_fftw[i].re)*(out_fftw[i].re) + (out_fftw[i].im) * (out_fftw[i].im);
        out_fftw_graf[i] = sqrt(out_fftw_graf[i]);
        //	qDebug() << "mass = "<< out_fftw_graf[i] << endl;
    }

    double N_min_YY=0, N_max_YY=0;
    for (i=0; i<N; i++){
        if (out_fftw_graf[i]<minYY) {minYY = out_fftw_graf[i]; N_min_YY=i;}
        if (out_fftw_graf[i]>maxYY) {maxYY = out_fftw_graf[i]; N_max_YY=i;}
        //	    XX[i] = N_max_YY;
    }
    if(N_max_YY >= fd/2)
        N_max_YY = N_max_YY - fd;

    for(i = 0; i< N; i++)
    {
        XX[i] = N_max_YY;
    }

    //for(i=0; i<N; i++)
    //	qDebug() << "XX[N_max] = "<< XX[i] << i <<endl;

    //	qDebug() << "min_YY = "<< minYY << endl;
    qDebug() << "max_YY = "<< maxYY << Qt::endl;
    //	qDebug() << "N_min_YY = "<< N_min_YY << endl;
    qDebug() << "N_max_YY = "<< N_max_YY << Qt::endl;

    ui -> widget -> xAxis -> setLabel("Spectr_Line");
    ui -> widget -> yAxis -> setLabel("A");
    ui -> widget -> yAxis -> setRange(minYY, maxYY);//Для оси Oy
    ui -> widget -> xAxis -> setRange(-fd/2, fd/2);
    ui -> widget -> xAxis -> grid()->setVisible(true);

    //        qDebug()<<__LINE__;
    //        ui -> widget -> xAxis -> setRange(-N/2, N/2);
    ui -> widget -> addGraph();
    ui -> widget -> graph(0)->setData(XX,out_fftw_graf);

    //        ui -> widget -> graph(0) -> rescaleAxes(true);
    ui -> widget -> graph(0) -> setPen(QPen(Qt::red)); // line color red for second graph
    //        ui -> widget -> clearGraphs();
    */
}

/* TODO: review */
void MainWindow::on_pbwritefile_clicked()
{
    qDebug() << "write clicked";
    int i = 0;
    int rnd = 0;
    int k = 0;
    short result[2];
    short inf_bit[N+10];
    cout << "N = "<< N +10 << endl;

    ui->progressBar->setMaximum(n2);
    ui->progressBar->setMinimum(n1);
    perek=ui->comboBox->currentIndex();
    switch(perek){
        case 0:{
            fileoutcarrier = fopen("resultcarrier.dat", "w+b");
            if (fileoutcarrier == NULL){
                qDebug() << "carrier file NOT opened";
            }
            else{
                qDebug() << "carrier file opened";
            }
            for (i=n1;i<=n2;i++){
                Re = A*(cos(2*PI*f*i/fd));
                Im = A*(sin(2*PI*f*i/fd));
                result[0]=Re;
                result[1]=Im;
                if (fileoutcarrier == NULL){
                    qDebug() << "error opening carrier";
                }
                else{
                    fwrite(result, sizeof(short), 2, fileoutcarrier);
                }
                ui->progressBar->setValue(i);
            }
            fclose(fileoutcarrier);
//            qDebug() << __LINE__;
        break;
    }
    case 1:{
            fileoutfsk = fopen("resultfsk.dat","w+b");
            if ((fileoutfsk == NULL)){
                qDebug() << "fsk file NOT opened";
            }
            else{
                qDebug() << "fsk file opened";
            }
	    k_rate=(rate * 200) / (rate * 4);
		cout << "rate= " << rate << endl;
                qDebug() << "k_rate = "<< k_rate << endl;

	    short inf_bit[N+10];
                cout << "N = "<< N +10 << endl;

	    k = 0; rnd = 0;
	if(meandr == 0)
	{    
    	    rnd = rand()%2;	    
	    for(i=n1; i<=n2; i++)
		{
                 inf_bit[i] = rnd;
		if(k==k_rate)
		  {
		   rnd = rand()%2;
		   k = 0;    
	          }
		else
                      k++;                  
	   	}
         }
	    else
               {
		for(i=n1; i<=n2; i++)
		{
                 inf_bit[i] = rnd;
		if(k==k_rate)
		  {
		   rnd = !rnd;
		   k = 0;    
	          }
		else
                     k++; 
             	}
               }

            for (i=n1;i<=n2;i++){
                switch(inf_bit[i]){
                    case 0:
                        Re = A * (cos(2*PI*(-df)*i/fd));
                        Im = A * (sin(2*PI*(-df)*i/fd));
                    break;
                    case 1:
                        Re = A * (cos(2*PI*(df)*i/fd));
                        Im = A * (sin(2*PI*(df)*i/fd));
                    break;
                    default:
                        qDebug() << "default case in frequency modulation";
                    break;
                }
                result[0]=Re;
                result[1]=Im;
                if (fileoutfsk == NULL){
                    qDebug() << "error opening fsk";
                }
                else{
                    fwrite(result, sizeof(short), 2, fileoutfsk);
                }
                ui->progressBar->setValue(i);
            }
            fclose(fileoutfsk);
//            qDebug() << __LINE__;
        break;
    }
        case 2:{
            fileout8psk = fopen("result8psk.dat","w+b");
            if (fileout8psk == NULL){
                qDebug() << "8psk file NOT opened";
            }
            else{
                qDebug() << "8psk file opened";
            }
	    k_rate=((rate/10) * 8);
	    k = 0;
	    if(meandr == 0)
             {
	    rnd = rand()%8;	    
	    for(i=n1; i<=n2; i++)
		{
                 inf_bit[i] = rnd;
		if(k==k_rate)
		  {
		   rnd = rand()%8;
//                   qDebug() << "rnd = " << rnd << endl;
		   k = 0;    
	          }
		else
		    {
                      k++;                  
                      qDebug() << "k = " << k << endl;
	   	    }
		}
            }
	     else
                {
	    	for(i=n1; i<=n2; i++)
	     	  { 
                    inf_bit[i] = rnd;
	            if(k==k_rate)
	              {
	                rnd++;
			if(rnd == 8)
                                    rnd = 0;
	     	        k = 0;    
	              }
		      else
                           k++; 
                  }
               	}
			

            // int rnd[N+10];
            for (i=n1;i<=n2;i++){
            //    rnd[i]=rand()%8;/*os[n].digitValue()*/
                switch(inf_bit[i]){
                    case 0:
                        Re = A* (cos(0));
                        Im = A* (sin(0));
                    break;
                    case 1:
                        Re = A* (cos(PI/4));
                        Im = A* (sin(PI/4));
                    break;
                    case 2:
                        Re = A* (cos(PI/2));
                        Im = A* (sin(PI/2));
                    break;
                    case 3:
                        Re = A* (cos(3*PI/4));
                        Im = A* (sin(3*PI/4));
                    break;
                    case 4:
                        Re = A* (cos(PI));
                        Im = A* (sin(PI));
                    break;
                    case 5:
                        Re = A* (cos(5*PI/4));
                        Im = A* (sin(5*PI/4));
                    break;
                    case 6:
                        Re = A* (cos(3*PI/2));
                        Im = A* (sin(3*PI/2));
                    break;
                    case 7:
                        Re = A* (cos(7*PI/4));
                        Im = A* (sin(7*PI/4));
                    break;
                    default:
                        qDebug()<<"error generating random numbers";
                    break;
                }
                result[0]=Re;
                result[1]=Im;
                ui->progressBar->setValue(i);
                if (fileout8psk == NULL){
                    qDebug() << "error opening 8psk";
                }
                else{
                    fwrite(result, sizeof(short), 2, fileout8psk);
                }
            }
            fclose(fileout8psk);
        break;
    }
        default:
            qDebug() << "something went wrong(maybe generating random numbers(0/1)";
        break;
        }
}

/* TODO: review */
void MainWindow::on_pbclean_clicked()
{
        qDebug() << "clean clicked";
        ui->widget->clearPlottables();
        ui->widget->replot();
        ui->progressBar->setValue(0);
        QSettings sett("/media/sf_general/praktika/praktika/params.ini", QSettings::IniFormat);
        sett.remove("os");

 //       ui->widget->rescaleAxes(true);
}

/* TODO: review */
void MainWindow::on_pbclose_clicked()
{
    qDebug() << "close clicked";
    close();
}

/* TODO: review */
void MainWindow::on_pbremovefile_clicked()
{
        qDebug() << "remove files clicked";
        perek=ui->comboBox->currentIndex();
        switch(perek){
            case 0:
                remove("resultcarrier.dat");
            break;
            case 1:
                remove("resultfsk.dat");
            break;
            case 2:
                remove("result8psk.dat");
            break;
            default:
                qDebug() << "case error";
            break;
        qDebug() << "removed files";
        }
}
