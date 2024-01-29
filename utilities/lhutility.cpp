#include "lhutility.h"

#if __linux__
    #include <unistd.h>
#else
    #include <windows.h>
#endif

LHUtility::LHUtility()
{

}

void LHUtility::intervalGenMs(unsigned int interval)
{
//    QEventLoop loop;
//    QTimer::singleShot(interval, &loop, SLOT(quit()));
//    loop.exec();
#if __linux__
    usleep(interval*1000);
#else
    Sleep(interval);
#endif
}
