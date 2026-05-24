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

using namespace std;

const QString WINDOW_TITLE = "Modulation of signal";
const QString X_AXIS_TIME_LABEL = "Real is blue, Imaginary is red";
const QString X_AXIS_SPECTRUM_LABEL = "Spectrum line";
const QString Y_AXIS_LABEL = "A";

const char *RESULT0_FILE_PATH = "resultcarrier.dat";
const char *RESULT1_FILE_PATH = "resultfsk.dat";

const double PI = acos(-1.0);


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

/* my diff: now there is 2 buttons to 2 distinct functions */
void MainWindow::on_pbtime_clicked()
{
    /* my diff: updated debug message */
    qDebug() << "Plotting a graph" << Qt::endl;

    /* me diff: now params is a struct! */
    params.loadFromSettings();
    params.sanitize();

    /* my diff: clear! */
    ui -> widget -> clearGraphs();

    /* vectors for plotting the graph */
    /* x is an index on OX axis (can be considered as time */
    /* re is a common-mode component (OY axis, blue) */
    /* im is a quadrature component (OY axis, red) */
    QVector<double> x(params.N), re(params.N), im(params.N);

    /* my diff: fixed strange cycle */
    /* also now PI is a const! */
    for (int i = 0; i < params.N; ++i) {
        int X = params.n1 + i;
        x[i] = X;
        re[i] = params.A * (cos(2 * PI * params.f * X / params.fd));
        im[i] = params.A * (sin(2 * PI * params.f * X / params.fd));
    }

    /* setting min and max y for OY axis */
    /* my diff: two distinct cycles was united */
    minY = re[0], maxY = re[0];
    for (int i = 1; i < params.N; ++i) {
        if (re[i] < minY) minY = re[i];
        else if (re[i] > maxY) maxY = re[i];
        else { ; }

        if (im[i] < minY) minY = im[i];
        else if (im[i] > maxY) maxY = im[i];
        else { ; }
    }

    ui -> widget -> xAxis -> setLabel(X_AXIS_TIME_LABEL);
    ui -> widget -> xAxis -> setRange(params.n1, params.n2);

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

void MainWindow::on_pbspectrum_clicked()
{
    qDebug() << "Spectrum chart";

    params.loadFromSettings();
    params.sanitize();

    ui -> widget -> clearGraphs();

    QVector<double> out_fftw_graf(params.N);
    QVector<double> x(params.N);
    fftw_complex *in_fftw, *out_fftw;
    fftw_plan plan;

    in_fftw = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * params.N);
    out_fftw = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * params.N);

    for (int i = 0; i < params.N; ++i) {
        int X = params.n1 + i;
        in_fftw[i][0] = params.A * (cos(2 * PI * params.f * X / params.fd));
        in_fftw[i][1] = params.A * (sin(2 * PI * params.f * X / params.fd));
    }

    plan = fftw_plan_dft_1d(params.N, in_fftw, out_fftw, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    for (int i = 0; i < params.N; ++i) {
        out_fftw_graf[i] = (out_fftw[i][0])*(out_fftw[i][0]) + (out_fftw[i][1])*(out_fftw[i][1]);
        out_fftw_graf[i] = sqrt(out_fftw_graf[i]);

        x[i] = (i < params.N/2) ? (i * params.fd / params.N) : ((i - params.N) * params.fd / params.N);
    }

    minY = out_fftw[0][0], maxY = out_fftw[0][0];
    for (int i = 0; i < params.N; ++i){
        if (out_fftw_graf[i] < minY) minY = out_fftw_graf[i];
        else if (out_fftw_graf[i] > maxY) maxY = out_fftw_graf[i];
        else { ; }
    }

    /* my diff: there was a wrong attempt to display negative frequencies */
    /* i deleted it, but probably we should make it right */

    /* my diff: magic string was replaced with const */
    ui -> widget -> xAxis -> setLabel(X_AXIS_SPECTRUM_LABEL);
    ui -> widget -> xAxis -> setRange(-params.fd/2, params.fd/2);
    ui -> widget -> xAxis -> grid() -> setVisible(true);

    ui -> widget -> yAxis -> setLabel(Y_AXIS_LABEL);
    ui -> widget -> yAxis -> setRange(minY, maxY);

    ui -> widget -> addGraph();
    ui -> widget -> graph(0) -> setData(x, out_fftw_graf);
    ui -> widget -> graph(0) -> setPen(QPen(Qt::red));

    ui -> widget -> replot();

    fftw_destroy_plan(plan);
    fftw_free(in_fftw);
    fftw_free(out_fftw);
}

/* TODO: review */
void MainWindow::on_pbwritefile_clicked()
{
    qDebug() << "Writing file" << Qt::endl;

    int i = 0;
    int rnd = 0;
    int k = 0;
    short result[2]; /* idk why there is short. probably the other program needs it. */
    /* i think we can do conditional compilation, but should we? */

    /* my diff: now using a dynamic array, `cause the size is unknown while compile */
    size_t size = params.N + 10;
    short *inf_bit = new short[size];

    /* my diff: i guess it must shows the size of the inf_bit array, so */
    qDebug() << "Size is " << size << Qt::endl;

    ui -> progressBar -> setMaximum(params.n2);
    ui -> progressBar -> setMinimum(params.n1);
    mode = ui -> comboBox -> currentIndex();

    switch(mode) {
        case 0: {
            /* my diff: updated magic string */
            fileoutcarrier = fopen(RESULT0_FILE_PATH, "w+b");

            /* my diff: normal debug! */
            if (fileoutcarrier == NULL){
                qDebug() << "Error during opening file";
                return;
            }
            else {
                qDebug() << "File opened correctly";
            }

            for (int i = params.n1; i <= params.n2; ++i) {
                Re = params.A * (cos(2 * PI * params.f * i / params.fd));
                Im = params.A * (sin(2 * PI * params.f * i / params.fd));
                result[0] = Re;
                result[1] = Im;
                fwrite(result, sizeof(short), 2, fileoutcarrier);
                ui -> progressBar -> setValue(i);
            }

            fclose(fileoutcarrier);
            break;
        }

        case 1: {
            fileoutfsk = fopen(RESULT1_FILE_PATH,"w+b");

            if (fileoutfsk == NULL){
                qDebug() << "Error during opening file";
                return;
            }
            else {
                qDebug() << "File opened correctly";
            }

	    k_rate=(rate * 200) / (rate * 4);
		cout << "rate= " << rate << endl;
                qDebug() << "k_rate = "<< k_rate << endl;

	    short inf_bit[N+10];
                cout << "N = "<< N +10 << endl;

	    k = 0; rnd = 0;

        /* my diff: srand added! absence of it was a big error btw */
        srand(time(0));
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
            //    rnd[i]=rand()%8;/*os[n].digitValue()
                /* WHATA HEEELL */
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
        }*/
}

/* TODO: review */
void MainWindow::on_pbremovefile_clicked()
{
    qDebug() << "remove files clicked";
    mode=ui->comboBox->currentIndex();
    switch(mode){
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

void MainWindow::on_pbclean_clicked()
{
    qDebug() << "Cleaning" << Qt::endl;

    ui -> widget -> clearPlottables();
    ui -> widget -> replot();
    ui -> progressBar -> setValue(0);
}

void MainWindow::on_pbclose_clicked()
{
    qDebug() << "Closing" << Qt::endl;

    close();
}
