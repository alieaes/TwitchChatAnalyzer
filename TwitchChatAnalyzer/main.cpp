#include "stdafx.h"
#include "cTwitchChatAnalyzer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cTwitchChatAnalyzer w;
    w.show();
    return a.exec();
}
