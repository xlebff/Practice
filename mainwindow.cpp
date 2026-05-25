#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QSettings>
#include <QVariant>
#include <QDebug>
#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

using namespace std;

const QString WINDOW_TITLE = "Modulation of signal";
const QString X_AXIS_TIME_LABEL = "Real is blue, Imaginary is red";
const QString X_AXIS_SPECTRUM_LABEL = "Spectrum line";
const QString Y_AXIS_LABEL = "A";

const char *CARRIER_MODE_RESULT_FILE_PATH = "resultcarrier.dat";
const char *FREQUENCY_MODE_RESULT_FILE_PATH = "resultfsk.dat";
const char *PHASE_MODE_RESULT_FILE_PATH = "result8psk.dat";

const int CARRIER_MODE = 0;
const int FREQUENCY_MODE = 1;
const int PHASE_MODE = 2;

const double PI = acos(-1.0);
const int PHASES_COUNT = 8;
const double PHASES[PHASES_COUNT] = {(0), (PI / 4), (PI / 2), (3 * PI / 4),
                                     (PI), (5 * PI / 4), (3 * PI / 2), (7 * PI / 4)};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui -> setupUi(this);
    ui -> progressBar -> setValue(0);
    this -> setWindowTitle(WINDOW_TITLE);

    srand(time(0));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbtime_clicked()
{
    qDebug() << "Plotting a graph" << Qt::endl;

    params.loadFromSettings();
    params.sanitize(this);

    ui -> widget -> clearGraphs();

    QVector<double> x(params.N), re(params.N), im(params.N);

    for (int i = 0; i < params.N; ++i) {
        int X = params.n1 + i;
        x[i] = X;
        re[i] = params.A * (cos(2 * PI * params.f * X / params.fd));
        im[i] = params.A * (sin(2 * PI * params.f * X / params.fd));
    }

    double minY = qMin(re[0], im[0]);
    double maxY = qMax(re[0], im[0]);
    for (int i = 1; i < params.N; ++i) {
        if (re[i] < minY) minY = re[i];
        else if (re[i] > maxY) maxY = re[i];

        if (im[i] < minY) minY = im[i];
        else if (im[i] > maxY) maxY = im[i];
    }

    ui -> widget -> xAxis -> setLabel(X_AXIS_TIME_LABEL);
    ui -> widget -> xAxis -> setRange(params.n1, params.n2);

    ui -> widget -> yAxis -> setLabel(Y_AXIS_LABEL);
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
    params.sanitize(this);

    ui -> widget -> clearGraphs();

    QVector<double> out_fftw_graf(params.N), x(params.N);
    fftw_complex *in_fftw, *out_fftw;
    fftw_plan plan;

    in_fftw = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * params.N);
    out_fftw = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * params.N);

    if (!in_fftw || !out_fftw) {
        QMessageBox::critical(this, "Error", "Error during memory allocation");
        qDebug() << "Error during memory allocation: " << Qt::endl <<
            "In FFTW: " << in_fftw << ";" << Qt::endl <<
            "Out FFTW: " << out_fftw << Qt::endl;
        return;
    }

    for (int i = 0; i < params.N; ++i) {
        double arg = 2 * PI * params.f * (i + params.n1) / params.fd;
        in_fftw[i][0] = params.A * (cos(arg));
        in_fftw[i][1] = params.A * (sin(arg));
    }

    plan = fftw_plan_dft_1d(params.N, in_fftw, out_fftw, FFTW_FORWARD, FFTW_ESTIMATE);
    if (!plan) {
        QMessageBox::critical(this, "Error", "FFTW plan creation failed");
        fftw_free(in_fftw);
        fftw_free(out_fftw);
        return;
    }

    fftw_execute(plan);

    for (int i = 0; i < params.N; ++i) {
        out_fftw_graf[i] = (out_fftw[i][0])*(out_fftw[i][0]) + (out_fftw[i][1])*(out_fftw[i][1]);
        out_fftw_graf[i] = sqrt(out_fftw_graf[i]);

        double df = static_cast<double>(params.fd) / params.N;
        x[i] = (i < params.N/2) ? i * df : (i - params.N) * df;
    }

    double minY = out_fftw_graf[0], maxY = out_fftw_graf[0];
    for (int i = 0; i < params.N; ++i){
        if (out_fftw_graf[i] < minY) minY = out_fftw_graf[i];
        else if (out_fftw_graf[i] > maxY) maxY = out_fftw_graf[i];
    }

    ui -> widget -> xAxis -> setLabel(X_AXIS_SPECTRUM_LABEL);
    ui -> widget -> xAxis -> setRange(-params.fd / 2, params.fd / 2);
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

void MainWindow::on_pbwritefile_clicked()
{
    qDebug() << "Writing file" << Qt::endl;

    params.loadFromSettings();
    params.sanitize(this);

    short result[2];

    ui -> progressBar -> setRange(0, params.N - 1);
    int mode = ui -> comboBox -> currentIndex();

    switch (mode) {
    case CARRIER_MODE:
    {
        FILE* f = fopen(CARRIER_MODE_RESULT_FILE_PATH, "w+b");
        if (!f) {
            QMessageBox::critical(this, "Error", "Error during opening carrier mode result file");
            qDebug() << "Error during opening " << CARRIER_MODE_RESULT_FILE_PATH << Qt::endl;
            return;
        }

        for (int i = 0; i < params.N; ++i) {
            double arg = 2 * PI * params.f * (i + params.n1) / params.fd;
            double re = params.A * cos(arg);
            double im = params.A * sin(arg);

            result[0] = re;
            result[1] = im;

            size_t written = fwrite(result, sizeof(short), 2, f);
            if (written != 2) {
                QMessageBox::warning(this, "Error", "Error during writting in carrier mode");
                qDebug() << "Error during writting in carrier mode" << Qt::endl;
            }

            ui -> progressBar -> setValue(i);
        }

        fclose(f);
        break;
    }

    case FREQUENCY_MODE:
    {
        FILE* f = fopen(FREQUENCY_MODE_RESULT_FILE_PATH, "w+b");
        if (!f) {
            QMessageBox::critical(this, "Error", "Error during opening frequency mode result file");
            qDebug() << "Error during opening " << FREQUENCY_MODE_RESULT_FILE_PATH << Qt::endl;
            return;
        }

        QVector<short> inf_bit(params.N);

        params.recalcSamples();

        inf_bit[0] = rand() % 2;
        for (int i = 1; i < params.N; ++i) {
            if (i % params.samplesPerSymbol == 0) {
                switch (params.meandr) {
                case MEANDR_MODE_RANDOM:
                    inf_bit[i] = rand() % 2;
                    break;
                case MEANDR_MODE_TOGGLE:
                    inf_bit[i] = inf_bit[i - 1] ^ 1;
                    break;
                }
            } else {
                inf_bit[i] = inf_bit[i - 1];
            }
        }

        for (int i = 0; i < params.N; ++i) {
            double freq = inf_bit[i] == 0 ?
                          params.f - params.df :
                          params.f + params.df;
            double arg = 2 * PI * freq * (i + params.n1) / params.fd;

            double re = params.A * cos(arg);
            double im = params.A * sin(arg);

            result[0] = re;
            result[1] = im;

            size_t written = fwrite(result, sizeof(short), 2, f);
            if (written != 2) {
                QMessageBox::warning(this, "Warning", "Error during writting in frequency mode");
                qDebug() << "Error during writting in frequency mode" << Qt::endl;
            }

            ui -> progressBar -> setValue(i);
        }

        fclose(f);
        break;
    }

    case PHASE_MODE:
    {
        FILE* f = fopen(PHASE_MODE_RESULT_FILE_PATH, "w+b");
        if (!f) {
            QMessageBox::critical(this, "Error", "Error during opening phase mode result file");
            qDebug() << "Error during opening " << PHASE_MODE_RESULT_FILE_PATH << Qt::endl;
            return;
        }

        QVector<short> inf_bit(params.N);

        params.recalcSamples();

        inf_bit[0] = rand() % 8;
        for (int i = 1; i < params.N; ++i) {
            if (i % params.samplesPerSymbol == 0) {
                switch (params.meandr) {
                case MEANDR_MODE_RANDOM:
                    inf_bit[i] = rand() % 8;
                    break;
                case MEANDR_MODE_TOGGLE:
                    inf_bit[i] = (inf_bit[i - 1] + 1) % 8;
                    break;
                }
            } else {
                inf_bit[i] = inf_bit[i - 1];
            }
        }

        for (int i = 0; i < params.N; ++i) {
            double re = params.A * cos(PHASES[inf_bit[i] % PHASES_COUNT]);
            double im = params.A * sin(PHASES[inf_bit[i] % PHASES_COUNT]);

            result[0] = re;
            result[1] = im;

            ui -> progressBar -> setValue(i);

            size_t written = fwrite(result, sizeof(short), 2, f);
            if (written != 2) {
                QMessageBox::warning(this, "Warning", "Error during writting in phase mode");
                qDebug() << "Error during writting in phase mode" << Qt::endl;
            }
        }

        fclose(f);
        break;
    }

    default:
        qDebug() << "Default case in modes" << Qt::endl;
        return;
    }
}

void MainWindow::on_pbremovefile_clicked()
{
    qDebug() << "Removing files" << Qt::endl;

    int mode = ui -> comboBox -> currentIndex();

    switch (mode) {
        case CARRIER_MODE:
            remove(CARRIER_MODE_RESULT_FILE_PATH);
            break;
        case FREQUENCY_MODE:
            remove(FREQUENCY_MODE_RESULT_FILE_PATH);
            break;
        case PHASE_MODE:
            remove(PHASE_MODE_RESULT_FILE_PATH);
            break;
        default:
            qDebug() << "Removing file case default" << Qt::endl;;
            break;
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
