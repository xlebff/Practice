#pragma once

#include <QMainWindow>
#include <qcustomplot.h>
#include <fftw3.h>
#include <unistd.h>
#include "SignalParams.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_pbtime_clicked();
        void on_pbspectrum_clicked();
        void on_pbclose_clicked();
        void on_pbwritefile_clicked();
        void on_pbclean_clicked();
        void on_pbremovefile_clicked();

    private:
        Ui::MainWindow *ui;
        SignalParams params;
};
