#pragma once

#include <QSettings>
#include <QDebug>
#include <QString>
#include <QFile>

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

const QString PATH ="params.ini";
const QString PARAMS = "Params";

const int MEANDR_MODE_RANDOM = 0;
const int MEANDR_MODE_TOGGLE = 1;


struct SignalParams {
    int A = A_DEFAULT;
    int f = F_DEFAULT;
    int fd = FD_DEFAULT;
    int n1 = N1_DEFAULT;
    int n2 = N2_DEFAULT;
    int df = DF_DEFAULT;
    int rate = RATE_DEFAULT;
    int meandr = MEANDR_DEFAULT;
    int N = 0;
    int samplesPerSymbol = 0;

    void loadFromSettings(const QString &path = PATH) {
        QSettings sett(path, QSettings::IniFormat);

        qDebug() << "Looking for file: " << sett.fileName();
        qDebug() << "File exists: " << QFile::exists(PATH) << Qt::endl;

        sett.beginGroup(PARAMS);
        this -> A = sett.value("A", A_DEFAULT).toInt();
        this -> f = sett.value("f", F_DEFAULT).toInt();
        this -> fd = sett.value("fd", FD_DEFAULT).toInt();
        this -> n1 = sett.value("n1", N1_DEFAULT).toInt();
        this -> n2 = sett.value("n2", N2_DEFAULT).toInt();
        this -> df = sett.value("df", DF_DEFAULT).toInt();
        this -> rate = sett.value("rate", RATE_DEFAULT).toInt();
        this -> meandr = sett.value("meandr", MEANDR_DEFAULT).toInt();
        sett.endGroup();
    }

    void recalcN()
    {
        N = n2 - n1;
    }

    void recalcSamples()
    {
        /* my diff: replaced k_rate */
        samplesPerSymbol = fd / rate;
        /* aand validation */
        if (samplesPerSymbol < 1) samplesPerSymbol = 1;

        /* aand debug */
        qDebug() << "Rate is " << rate << Qt::endl;
        qDebug() << samplesPerSymbol << " samples per symbol" << Qt::endl;
    }

    void sanitize()
    {
        normalize(A, A_MIN, A_MAX, "A");
        normalize(fd, FD_MIN, FD_MAX, "Fd");
        normalize(f, F_MIN, fd / 2, "F");
        normalize(n1, N1_MIN, N1_MAX, "N1");
        normalize(n2, N2_MIN + n1, N2_MAX, "N2");
        normalize(df, DF_MIN, fd / 2, "Df");
        normalize(rate, RATE_MIN, fd / 2, "Rate");
        normalize(meandr, MEANDR_MIN, MEANDR_MAX, "Meandr");

        recalcN();

        if (N <= 0) {
            qDebug() << "Error during calculating N!" << Qt::endl;
            exit(1);
        }

        if (rate > fd) {
            rate = fd;
            qDebug() << "Rate reduced to fd" << Qt::endl;
        }
    }

    /* my diff: method for normalize params. was updated and now shows what var was fixed. */
    /* also i think it has to show debug in the widget */
    void normalize(int &val, const int min, const int max, const QString name)
    {
        if (val < min) {
            val = min;
            qDebug() << name << " must be greater than " <<
                min << ". Value was normalized." << Qt::endl;
        }
        else if (val > max) {
            val = max;
            qDebug() << name << " must be less than " <<
                max << ". Value was normalized" << Qt::endl;
        }
    }
};
