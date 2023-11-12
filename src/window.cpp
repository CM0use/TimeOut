// TimeOut
// Copyright (c) 2023 CM0use

#include "window.h"
#include "icons.h"

#include <QApplication>
#include <QSettings>
#include <QToolButton>
#include <QMenu>
#include <QLabel>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <QProcess>
#include <QClipboard>
#include <QFile>

Window::Window(QWidget *const parent)
    : FramelessWindow{ parent },
    appName{ qApp->applicationName() },
    appVersion{ qApp->applicationVersion() }
{
    initializeMembers();

    createTitleBar();
    createMenus();
    createActions();
    configurateActions();
    configurateMenus();
    configurateTitleBar();

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        createTrayIcon();
        configurateTrayIcon();
    }

    updateIcons();
    connectSignals();
}

void Window::closeEvent(QCloseEvent *const)
{
    saveSettings();
    QApplication::postEvent(qApp, new QEvent(QEvent::Quit));
}

qsizetype Window::getCurrentIndexTheme()
{
    return currentIndexTheme;
}

void Window::updateIcons()
{
    if (currentIndexTheme) {
        popUpButton->setIcon(MENU_ICON_LIGHT);

        languagesMenu->setIcon(TRANSLATE_ICON_LIGHT);
        languageActionList[currentIndexLanguage]->setIcon(CHECK_ICON_LIGHT);

        themesMenu->setIcon(PALETTE_ICON_LIGHT);
        themeActionList[currentIndexTheme]->setIcon(CHECK_ICON_LIGHT);

        customTitleBarAction->setIcon(customTitleBarAction->isChecked()? DONE_ICON_LIGHT: QIcon());

        helpMenu->setIcon(HELP_ICON_LIGHT);
        aboutAction->setIcon(INFO_ICON_LIGHT);
        creditsAction->setIcon(COPYRIGHT_ICON_LIGHT);

        exitAction->setIcon(EXIT_ICON_LIGHT);

        appIcon->setPixmap(APP_ICON_LIGHT.pixmap(35));

        minimizeButton->setIcon(MINIMIZE_ICON_LIGHT);
        closeButton->setIcon(CLOSE_ICON_LIGHT);
        if(QSystemTrayIcon::isSystemTrayAvailable())
            showProgramAction->setIcon(APP_ICON_LIGHT);
    } else {
        popUpButton->setIcon(MENU_ICON_DARK);

        languagesMenu->setIcon(TRANSLATE_ICON_DARK);
        languageActionList[currentIndexLanguage]->setIcon(CHECK_ICON_DARK);

        themesMenu->setIcon(PALETTE_ICON_DARK);
        themeActionList[currentIndexTheme]->setIcon(CHECK_ICON_DARK);

        customTitleBarAction->setIcon(customTitleBarAction->isChecked()? DONE_ICON_DARK: QIcon());

        helpMenu->setIcon(HELP_ICON_DARK);
        aboutAction->setIcon(INFO_ICON_DARK);
        creditsAction->setIcon(COPYRIGHT_ICON_DARK);

        exitAction->setIcon(EXIT_ICON_DARK);

        appIcon->setPixmap(APP_ICON_DARK.pixmap(35));

        minimizeButton->setIcon(MINIMIZE_ICON_DARK);
        closeButton->setIcon(CLOSE_ICON_DARK);
        if(QSystemTrayIcon::isSystemTrayAvailable())
            showProgramAction->setIcon(APP_ICON_DARK);
    }
}

void Window::initializeMembers()
{
    userSettings = new QSettings(QCoreApplication::applicationDirPath()
                                     + "/config/Settings.ini",
                                 QSettings::Format::IniFormat);

    currentIndexLanguage = userSettings->value("Settings/IndexLanguage").toInt();
    currentIndexTheme = userSettings->value("Settings/IndexTheme").toInt();
}

void Window::createTitleBar()
{
    titleBar = new QWidget(this);

    popUpButton = new QToolButton(titleBar),
        appMenu = new QMenu(popUpButton);

    appIcon = new QLabel(titleBar);
    appTitleName = new QLabel(appName, titleBar);

    minimizeButton = new QToolButton(titleBar);
    closeButton = new QToolButton(titleBar);
}

void Window::createMenus()
{
    languagesMenu = new QMenu(tr("&Languages"), appMenu);
    themesMenu = new QMenu(tr("&Themes"), appMenu);
    helpMenu = new QMenu(tr("&Help"), appMenu);
}

void Window::createActions()
{
    languageActionList ={ new QAction(tr("[en] English"), languagesMenu),
                          new QAction(tr("[es] Español"), languagesMenu) };

    themeActionList ={ new QAction(tr("Light"), themesMenu),
                       new QAction(tr("Dark"), themesMenu) };

    customTitleBarAction = new QAction(tr("&Custom Title Bar"), appMenu);

    aboutAction = new QAction(tr("&About %1").arg(appName), helpMenu);
    creditsAction = new QAction(tr("&Credits"), helpMenu);
    aboutQtAction = new QAction(tr("About &Qt"), helpMenu);

    exitAction = new QAction(tr("E&xit"), appMenu);
}

void Window::configurateActions()
{
    themeActionList[0]->setObjectName("light.css");
    themeActionList[1]->setObjectName("dark.css");

    customTitleBarAction->setCheckable(true);
    customTitleBarAction->setChecked(true);

    aboutQtAction->setIcon(QT_ICON);

    exitAction->setShortcut(QKeySequence::Quit);
}

void Window::configurateMenus()
{
    languagesMenu->addActions(languageActionList);

    themesMenu->addActions(themeActionList);

    helpMenu->addAction(aboutAction);
    helpMenu->addAction(creditsAction);
    helpMenu->addAction(aboutQtAction);
}

void Window::configurateTitleBar()
{
    popUpButton->setPopupMode(QToolButton::InstantPopup);
    popUpButton->setAutoRaise(true);
    popUpButton->setMenu(appMenu);

    appMenu->addMenu(languagesMenu);
    appMenu->addMenu(themesMenu);
    appMenu->addAction(customTitleBarAction);
    appMenu->addMenu(helpMenu);
    appMenu->addSeparator();
    appMenu->addAction(exitAction);

    QHBoxLayout *const hLayoutBar{ new QHBoxLayout };
    hLayoutBar->setContentsMargins(0, 0, 0, 0);

    hLayoutBar->addWidget(popUpButton);
    hLayoutBar->addStretch();
    hLayoutBar->addWidget(appIcon);
    hLayoutBar->addWidget(appTitleName);
    hLayoutBar->addStretch();

    minimizeButton->setAutoRaise(true);
    closeButton->setAutoRaise(true);

    QHBoxLayout *const buttonsHLayout{ new QHBoxLayout };
    buttonsHLayout->setContentsMargins(0, 0, 0, 0);
    buttonsHLayout->setSpacing(0);
    buttonsHLayout->addWidget(minimizeButton);
    buttonsHLayout->addWidget(closeButton);

    hLayoutBar->addLayout(buttonsHLayout);

    titleBar->setLayout(hLayoutBar);
    setTitleBar(titleBar);
}

void Window::createTrayIcon()
{
    myTrayIcon = new QSystemTrayIcon(APP_ICON_LIGHT, this);
    myTrayIconMenu = new QMenu(this);
    showProgramAction = new QAction(tr("Show %1").arg(appName), myTrayIcon);
}

void Window::configurateTrayIcon()
{
    myTrayIconMenu->addAction(showProgramAction);
    myTrayIconMenu->addSeparator();
    myTrayIconMenu->addAction(exitAction);

    myTrayIcon->setContextMenu(myTrayIconMenu);
    myTrayIcon->show();
    myTrayIcon->setToolTip(appName);


    connect(myTrayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);
    connect(showProgramAction, &QAction::triggered, this, [this] {
        if (!isActiveWindow() || isHidden() || isMinimized()) {
            activateWindow();
            showNormal();
        }
    });

}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (!isActiveWindow() || isHidden() || isMinimized()) {
            activateWindow();
            showNormal();
        }
    }
}

void Window::connectSignals()
{
    for(const QAction *const languageAction: languageActionList)
        connect(languageAction, &QAction::triggered, this, &Window::changeLanguage);

    for(const QAction *const themeAction: themeActionList)
        connect(themeAction, &QAction::triggered, this, &Window::changeTheme);

    connect(customTitleBarAction, &QAction::triggered, this, [this] (bool checked) {
        setWindowFlag(Qt::FramelessWindowHint, checked);
        setWindowFlag(Qt::WindowMaximizeButtonHint, checked);

        setTitleBar(checked? titleBar: nullptr);
        appIcon->setVisible(checked);
        appTitleName->setVisible(checked);
        minimizeButton->setVisible(checked);
        closeButton->setVisible(checked);
        customTitleBarAction->setIcon(checked? currentIndexTheme? DONE_ICON_LIGHT: DONE_ICON_DARK
                                              : QIcon());

        showNormal();
    });

    connect(aboutAction, &QAction::triggered, this, &Window::about);
    connect(creditsAction, &QAction::triggered, this, &Window::credits);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(exitAction, &QAction::triggered, this, &Window::close);

    connect(minimizeButton, &QToolButton::clicked,
            this, [this] { setWindowState(Qt::WindowMinimized); });
    connect(closeButton, &QToolButton::clicked, this, &Window::close);
}

void Window::changeLanguage()
{
    qsizetype senderLanguageIndex{ languageActionList.indexOf(static_cast<QAction *>(sender())) };

    const static qsizetype programLanguage{ currentIndexLanguage };

    if (programLanguage != senderLanguageIndex) {
        languageActionList[currentIndexLanguage]->setIcon(QIcon());
        languageActionList[senderLanguageIndex]
            ->setIcon(currentIndexTheme? WARNING_ICON_LIGHT: WARNING_ICON_DARK);
        currentIndexLanguage = senderLanguageIndex;
        restart();
    } else {
        languageActionList[currentIndexLanguage]->setIcon(QIcon());
        languageActionList[senderLanguageIndex]
            ->setIcon(currentIndexTheme? CHECK_ICON_LIGHT: CHECK_ICON_DARK);
        currentIndexLanguage = senderLanguageIndex;
    }
}

void Window::restart()
{
    QMessageBox question{ this };
    question.setWindowTitle(tr("Restart Required"));
    question.setText(tr("You will need to restart in order to"
                        " use your new language setting\n"));
    question.addButton(tr("Restart Now"), QMessageBox::YesRole);
    question.buttons().at(0)->setIcon(currentIndexTheme? REFRESH_ICON_LIGHT: REFRESH_ICON_DARK);
    question.addButton(tr("Restart Later"), QMessageBox::NoRole);
    question.buttons().at(1)->setIcon(currentIndexTheme? OFF_ICON_LIGHT: OFF_ICON_DARK);
    bool isRestartNow{ !question.exec() };
    if (isRestartNow) {
        QProcess process;
        const QString program{ qApp->arguments()[0] },
            workingDirectory{ qApp->applicationDirPath() };
        QStringList arguments{ qApp->arguments() };
        arguments.removeAt(0);
        bool processHasError{ !process.startDetached(program, arguments, workingDirectory) };
        if (processHasError) {
            QProcess::ProcessError error{ process.error() };
            QMessageBox errorBox{ this };
            errorBox.setWindowTitle(tr("Error"));
            errorBox.setText(tr("Error: \"enum QProcess::ProcessError\"\n"
                                "Value error: %1\n"
                                "The program can still be used\n"
                                "but cannot be restarted due to an error.\n\n"
                                "Do you want copy this error "
                                "in your clipboad?").arg(error));
            errorBox.addButton(tr("Copy to clipboard"), QMessageBox::YesRole);
            errorBox.buttons().at(0)->setIcon(currentIndexTheme? COPY_ICON_LIGHT: COPY_ICON_DARK);
            errorBox.addButton(tr("No"), QMessageBox::NoRole);
            errorBox.buttons().at(1)->setIcon(currentIndexTheme? OFF_ICON_LIGHT: OFF_ICON_DARK);
            bool userDecisionIsYes{ !errorBox.exec() };

            if (userDecisionIsYes)
                QApplication::clipboard()->
                    setText(QString("enum QProcess::ProcessError: %1").arg(error));
        } else close();
    }
}

void Window::saveSettings()
{
    userSettings->setValue("Settings/IndexLanguage", currentIndexLanguage);
    userSettings->setValue("Settings/IndexTheme", currentIndexTheme);
}

void Window::changeTheme()
{
    qsizetype senderThemeIndex{ themeActionList.indexOf(static_cast<QAction *>(sender())) };
    if (senderThemeIndex != currentIndexTheme) {
        QFile styleSheetFile{ ":/qss/" + themeActionList[senderThemeIndex]->objectName() };
        styleSheetFile.open(QFile::ReadOnly);
        qApp->setStyleSheet(styleSheetFile.readAll());
        themeActionList[currentIndexTheme]->setIcon(QIcon());
        themeActionList[senderThemeIndex]->setIcon(currentIndexTheme? CHECK_ICON_LIGHT: CHECK_ICON_DARK);
        currentIndexTheme = senderThemeIndex;

        emit themeChanged(true);
    }
}

void Window::about()
{
    const static QString description
        { tr("<div style=\"text-align: center\">"
               "<h1>%1</h1>"
               "<p>Version: %2</p>"
               "<p>A timer with customized program/command executions</p>"
               "<p>"
               "The timer can be stopped and restarted. To restart it, "
               "you can specify a new hour, minute or second.<br>"
               "Optionally, a program can be executed after the timer is terminated.<br>"
               "The output of the program will be recorded in the text "
               "file specified in the \"Program output\" field, this text file will include two "
               "lines at the end with the output code and the program output status.</p>"
               "<p>For issues or contributions, visit the "
               "<a href=https://github.com/CM0use/GenPassword style=color:#0097CF>"
               "GitHub repository</a></p>"
               "<p>Copyright © 2023 - CM0use</p>"
               "<p>This program comes with NO WARRANTY OF ANY KIND.<br>"
               "See the <a href=https://www.gnu.org/licenses/gpl-3.0.html style=color:#0097CF>"
               "GNU General Public License</a> for details.</p>"
               "</div>").arg(appName, appVersion)
        };
    QMessageBox::about(this, tr("About %1").arg(appName), description);
}

void Window::credits()
{
    const static QString description
        { tr("<div style=\"text-align: center\">"
               "<h1>%1</h1>"
               "<p>Created by <a href=https://github.com/CM0use style=color:#0097CF>CM0use</a></p>"
               "<p>Contributors:</p>"
               "<ul>"
               "<li><a href=https://bard.google.com style=color:#0097CF>Google Bard</a> (Helper)</li>"
               "<li><a href=https://fonts.google.com/icons style=color:#0097CF>Material Icons</a></li>"
               "<li><a href=https://www.deepl.com/translator style=color:#0097CF>DeepL</a> (Translator)</li>"
               "</ul>"
               "</div>").arg(appName)
        };
    QMessageBox::about(this, tr("Credits"), description);
}
