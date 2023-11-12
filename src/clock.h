// TimeOut
// Copyright (c) 2023 CM0use

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "window.h"

#include <QProcess>
#include <QTime>

QT_BEGIN_NAMESPACE
class QLCDNumber;
class QSpinBox;
class QTime;
class QRadioButton;
class QLineEdit;
class QFile;
class QPushButton;
class QSoundEffect;
QT_END_NAMESPACE

class TimeOut final: public Window
{
public:
    explicit TimeOut(QWidget *const parent = nullptr);
    virtual ~TimeOut() = default;

private:
    void initializeMembers();

    void createWidgets();
    void configureWidgets();
    void createUi();
    void updateAllIcons(const bool allIcons = true);
    void connectSignals();

    void setTimer();
    void displayTimer();
    void startTimer(const bool checked);
    void updateStartTimerButtonState();

    void processProgramOutput();
    void processProgramFinished(const int exitCode,
                                const QProcess::ExitStatus exitStatus = QProcess::NormalExit);

    void disableWidgets(const bool block = true);
    void chooseFile();

    QTime time;
    QTimer *timer;
    QLCDNumber *clockLCD;

    QSpinBox *hourSpinBox, *minuteSpinBox, *secondSpinBox;
    QRadioButton *shutdownRadioButton, *rebootRadioButton, *doNothingRadioButton;

    QLineEdit *programLineEdit, *programArgsLineEdit, *programOutputLineEdit;
    QPair<QString, QString> userSelectedOption;
    QToolButton *selectProgramButton, *selectProgramArgsButton, *selectProgramOutputButton;
    QFile *programOutputFile;

    QPushButton *startTimerButton;
    bool isResetNeeded;

    QProcess *programProcess;
};

#endif // TIMEOUT_H
