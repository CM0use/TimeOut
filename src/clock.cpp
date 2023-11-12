// TimeOut
// Copyright (c) 2023 CM0use

#include "clock.h"
#include "icons.h"

#include <QTimer>
#include <QFile>
#include <QLCDNumber>
#include <QSpinBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>

TimeOut::TimeOut(QWidget *const parent)
    : Window{ parent }
{
    initializeMembers();
    createWidgets();
    configureWidgets();
    createUi();
    updateAllIcons(false);
    connectSignals();
}

void TimeOut::initializeMembers()
{
    time.setHMS(0, 0, 0);

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    programOutputFile = new QFile(this);

    isResetNeeded = false;

    programProcess = new QProcess(this);
    programProcess->setProcessChannelMode(QProcess::MergedChannels);
}

void TimeOut::createWidgets()
{
    clockLCD = new QLCDNumber;

    hourSpinBox = new QSpinBox;
    minuteSpinBox = new QSpinBox;
    secondSpinBox = new QSpinBox;

    shutdownRadioButton = new QRadioButton;
    rebootRadioButton = new QRadioButton;
    doNothingRadioButton = new QRadioButton;

    programLineEdit = new QLineEdit;
    selectProgramButton = new QToolButton;

    programArgsLineEdit = new QLineEdit;
    selectProgramArgsButton = new QToolButton;

    programOutputLineEdit = new QLineEdit;
    selectProgramOutputButton = new QToolButton;

    startTimerButton = new QPushButton;
}

void TimeOut::configureWidgets()
{
    clockLCD->setDigitCount(8);
    clockLCD->display(time.toString("hh:mm:ss"));
    clockLCD->setSegmentStyle(QLCDNumber::Flat);
    clockLCD->setMinimumHeight(150);

    hourSpinBox->setMaximum(23);
    hourSpinBox->setObjectName("hour");
    minuteSpinBox->setMaximum(59);
    minuteSpinBox->setObjectName("minute");
    secondSpinBox->setMaximum(59);
    secondSpinBox->setObjectName("second");

    shutdownRadioButton->setText(QObject::tr("Poweroff"));
    rebootRadioButton->setText(QObject::tr("Reboot"));
    doNothingRadioButton->setText(QObject::tr("Nothing"));
    doNothingRadioButton->setChecked(true);

#ifdef Q_OS_WIN
    programLineEdit->setPlaceholderText("C:/Program Files/myProgram/myprogram.exe");
    programArgsLineEdit->setPlaceholderText("C:/Users/user/Videos/myvideo.mp4");
    programOutputLineEdit->setPlaceholderText("C:/Users/user/Desktop/mylog.txt");
#else
    programLineEdit->setPlaceholderText("mpv");
    programArgsLineEdit->setPlaceholderText("~/Videos/myvideo.mp4");
    programOutputLineEdit->setPlaceholderText("~/Desktop/mylog.txt");
#endif

    selectProgramButton->setObjectName("program");
    selectProgramArgsButton->setObjectName("args");
    selectProgramOutputButton->setObjectName("output");

    startTimerButton->setIconSize(startTimerButton->sizeHint());
    startTimerButton->setCheckable(true);
    startTimerButton->setDisabled(true);
}

void TimeOut::updateAllIcons(bool allIcons)
{
    if (getCurrentIndexTheme()) {
        selectProgramButton->setIcon(FOLDER_ICON_LIGHT);
        selectProgramArgsButton->setIcon(FOLDER_ICON_LIGHT);
        selectProgramOutputButton->setIcon(FOLDER_ICON_LIGHT);

        startTimerButton->setIcon(startTimerButton->isChecked()? PAUSE_ICON_LIGHT: PLAY_ICON_LIGHT);
    } else {
        selectProgramButton->setIcon(FOLDER_ICON_DARK);
        selectProgramArgsButton->setIcon(FOLDER_ICON_DARK);
        selectProgramOutputButton->setIcon(FOLDER_ICON_DARK);
        startTimerButton->setIcon(startTimerButton->isChecked()? PAUSE_ICON_DARK: PLAY_ICON_DARK);
    }

    if(allIcons)
        Window::updateIcons();
}

void TimeOut::createUi()
{
    std::array<QHBoxLayout *const, 5> horizontalLayout
    {
        new QHBoxLayout,
        new QHBoxLayout,
        new QHBoxLayout,
        new QHBoxLayout,
        new QHBoxLayout
    };

    horizontalLayout[0]->addWidget(hourSpinBox);
    horizontalLayout[0]->addWidget(new QLabel(":"));
    horizontalLayout[0]->addWidget(minuteSpinBox);
    horizontalLayout[0]->addWidget(new QLabel(":"));
    horizontalLayout[0]->addWidget(secondSpinBox);

    horizontalLayout[1]->addWidget(shutdownRadioButton);
    horizontalLayout[1]->addWidget(rebootRadioButton);
    horizontalLayout[1]->addWidget(doNothingRadioButton);

    horizontalLayout[2]->addWidget(new QLabel(QObject::tr("Program:")));
    horizontalLayout[2]->addWidget(programLineEdit);
    horizontalLayout[2]->addWidget(selectProgramButton);

    horizontalLayout[3]->addWidget(new QLabel(QObject::tr("Program arguments:")));
    horizontalLayout[3]->addWidget(programArgsLineEdit);
    horizontalLayout[3]->addWidget(selectProgramArgsButton);

    horizontalLayout[4]->addWidget(new QLabel(QObject::tr("Program output:")));
    horizontalLayout[4]->addWidget(programOutputLineEdit);
    horizontalLayout[4]->addWidget(selectProgramOutputButton);

    QGridLayout *const mainGridlLayout{ new QGridLayout };
    mainGridlLayout->addWidget(getTitleBar());
    mainGridlLayout->addWidget(clockLCD);
    mainGridlLayout->addLayout(horizontalLayout[0], 2, 0, Qt::AlignCenter);
    mainGridlLayout->addLayout(horizontalLayout[1], 3, 0, Qt::AlignCenter);
    mainGridlLayout->addLayout(horizontalLayout[2], 4, 0);
    mainGridlLayout->addLayout(horizontalLayout[3], 5, 0);
    mainGridlLayout->addLayout(horizontalLayout[4], 6, 0);
    mainGridlLayout->addWidget(startTimerButton, 7, 0, Qt::AlignCenter);

    QWidget *const mainWidget{ new QWidget };
    mainWidget->setLayout(mainGridlLayout);
    setCentralWidget(mainWidget);
}

void TimeOut::connectSignals()
{
    connect(this, &Window::themeChanged, this, &TimeOut::updateAllIcons);

    for(QSpinBox *const timeSpinBox: { secondSpinBox, minuteSpinBox, hourSpinBox })
        connect(timeSpinBox, &QSpinBox::valueChanged, this, [this] {
            setTimer();
            isResetNeeded = true;
            updateStartTimerButtonState();
        });

    for(QToolButton *const selectButton: { selectProgramButton, selectProgramArgsButton, selectProgramOutputButton })
        connect(selectButton, &QToolButton::clicked, this, &TimeOut::chooseFile);

    connect(startTimerButton, &QPushButton::clicked, this, &TimeOut::startTimer);

    connect(timer, &QTimer::timeout, this, &TimeOut::displayTimer);

    connect(programProcess, &QProcess::readyRead, this, &TimeOut::processProgramOutput);
    connect(programProcess, &QProcess::finished, this, &TimeOut::processProgramFinished);
}

void TimeOut::chooseFile()
{
    QString nameOfButton{ static_cast<QToolButton *>(sender())->objectName() };

    static QFileDialog fileDialog;
    fileDialog.setDirectory(QDir::homePath());
    if (nameOfButton == "program")
        fileDialog.setFileMode(QFileDialog::ExistingFile);
    else fileDialog.setFileMode(QFileDialog::ExistingFiles);

    if (fileDialog.exec() == QDialog::Accepted) {
        static QStringList selectedFiles;
        selectedFiles = fileDialog.selectedFiles();
        std::reverse(selectedFiles.begin(), selectedFiles.end());

        if (nameOfButton == "program")
            programLineEdit->setText(selectedFiles[0]);
        else if (nameOfButton == "args")
            programArgsLineEdit->setText(selectedFiles.join(' '));
        else programOutputLineEdit->setText(selectedFiles.join(' '));
    }
}

void TimeOut::updateStartTimerButtonState()
{    
    if (!hourSpinBox->value() && !minuteSpinBox->value() && !secondSpinBox->value())
        startTimerButton->setDisabled(true);
    else startTimerButton->setDisabled(false);
}

void TimeOut::processProgramOutput()
{
    if (programOutputFile->isOpen()) {
        QString output{ QString("[%1]: ").arg(QTime::currentTime().toString())
                        + QString(programProcess->readAll()) };
        programOutputFile->write(output.toStdString().c_str());
    }
}

void TimeOut::processProgramFinished(const int exitCode, const QProcess::ExitStatus exitStatus)
{
    programProcess->close();

    if(programOutputFile->isOpen()) {
        QString outputStatus{ QString("\nExit code of the program: %1\n"
                               "Exit of process status: %2").arg(exitCode).arg(exitStatus) };
        programOutputFile->write(outputStatus.toStdString().c_str());
        programOutputFile->close();
    }

    if (userSelectedOption.first != "")
        programProcess->start(userSelectedOption.first, userSelectedOption.second.split(' '));
}

void TimeOut::displayTimer()
{
    clockLCD->display(time.toString("hh:mm:ss"));
    if (time.toString("hh:mm:ss") != "00:00:00") {
        myTrayIcon->setToolTip(QObject::tr("Time Left: %1").arg(time.toString("hh:mm:ss")));
        time = time.addSecs(-1);
    }
    else {
        timer->stop();

        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            myTrayIcon->setToolTip(appName);
            myTrayIcon->showMessage(QObject::tr("Time's up!"), QObject::tr("The countdown is over."),
                                    QSystemTrayIcon::MessageIcon::Information, 10000);
        }

#ifdef Q_OS_WIN
        if (shutdownRadioButton->isChecked() || rebootRadioButton->isChecked()) {
            userSelectedOption.first = "C:/Windows/System32/shutdown.exe";
            userSelectedOption.second = shutdownRadioButton->isChecked()? "/s":
                                        rebootRadioButton->isChecked()? "/r": "";
        }
#else
        userSelectedOption.first = shutdownRadioButton->isChecked()? "/sbin/poweroff":
                                   rebootRadioButton->isChecked()? "/sbin/reboot": "";
        userSelectedOption.second = "";
#endif
        isResetNeeded = true;

        if (!programLineEdit->text().isEmpty())
            programProcess->start(programLineEdit->text(),
                                  !programArgsLineEdit->text().isEmpty()?
                                      programArgsLineEdit->text().split(' '): QStringList());
        else programProcess->start(userSelectedOption.first,
                                  userSelectedOption.second.isEmpty()? QStringList():
                                  userSelectedOption.second.split(' '));

        disableWidgets(false);
    }
}

void TimeOut::setTimer()
{
    const QSpinBox *const spinBoxSender{ static_cast<QSpinBox *>(sender()) };

    const QString senderName{ spinBoxSender->objectName() };
    const int senderValue{ spinBoxSender->value() };

    if (senderName == "hour")
        time.setHMS(senderValue, minuteSpinBox->value(), secondSpinBox->value());
    else if (senderName == "minute")
        time.setHMS(hourSpinBox->value(), senderValue, secondSpinBox->value());
    else time.setHMS(hourSpinBox->value(), minuteSpinBox->value(), senderValue);

    clockLCD->display(time.toString("hh:mm:ss"));
}

void TimeOut::disableWidgets(const bool block)
{
    hourSpinBox->setDisabled(block);
    minuteSpinBox->setDisabled(block);
    secondSpinBox->setDisabled(block);

    shutdownRadioButton->setDisabled(block);
    rebootRadioButton->setDisabled(block);
    doNothingRadioButton->setDisabled(block);

    programLineEdit->setDisabled(block);
    selectProgramButton->setDisabled(block);

    programArgsLineEdit->setDisabled(block);
    selectProgramArgsButton->setDisabled(block);

    programOutputLineEdit->setDisabled(block);
    selectProgramOutputButton->setDisabled(block);

    if(getCurrentIndexTheme())
        startTimerButton->setIcon(block? PAUSE_ICON_LIGHT: PLAY_ICON_LIGHT);
    else startTimerButton->setIcon(block? PAUSE_ICON_DARK: PLAY_ICON_DARK);

    startTimerButton->setChecked(block);
}

void TimeOut::startTimer(const bool checked)
{
    if (checked) {
        disableWidgets();

        if (!programLineEdit->text().isEmpty() && !programOutputLineEdit->text().isEmpty()) {
            programOutputFile->setFileName(programOutputLineEdit->text());
            programOutputFile->open(QIODevice::Append);
        }

        if (isResetNeeded) {
            isResetNeeded = false;
            time.setHMS(hourSpinBox->value(), minuteSpinBox->value(), secondSpinBox->value());
            clockLCD->display(time.toString("hh:mm:ss"));
            displayTimer();
        }

        timer->start(1000);
    } else {
        timer->stop();
        if (programProcess->isOpen()) programProcess->close();
        if (programOutputFile->isOpen()) programOutputFile->close();
        disableWidgets(false);
    }
}
