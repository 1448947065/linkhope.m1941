#ifndef LHUTILITY_H
#define LHUTILITY_H

#include <QObject>
#include <QProcess>
#include <QTextCodec>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

class LHUtility
{
public:
    LHUtility();
    static void intervalGenMs(unsigned int interval);
};

#endif // LHUTILITY_H
