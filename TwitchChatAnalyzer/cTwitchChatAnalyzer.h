#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cTwitchChatAnalyzer.h"

class cTwitchChatAnalyzer : public QMainWindow
{
    Q_OBJECT

public:
    cTwitchChatAnalyzer(QWidget *parent = nullptr);
    ~cTwitchChatAnalyzer();

private:
    Ui::cTwitchChatAnalyzerClass ui;
};
