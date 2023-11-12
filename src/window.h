// TimeOut
// Copyright (c) 2023 CM0use

#ifndef WINDOW_H
#define WINDOW_H

#include "framelesswindow.h"

#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
class QToolButton;
class QLabel;
class QSettings;
QT_END_NAMESPACE

class Window: public FramelessWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget *const parent = nullptr);
    virtual ~Window() = default;

signals:
    void themeChanged(bool);

protected:
    virtual void closeEvent(QCloseEvent *const) override;

    qsizetype getCurrentIndexTheme();
    void updateIcons();

    QSystemTrayIcon *myTrayIcon;
    const QString appName, appVersion;

private:
    void initializeMembers();

    void createTitleBar();
    void createMenus();
    void createActions();
    void configurateActions();
    void configurateMenus();
    void configurateTitleBar();

    void createTrayIcon();
    void configurateTrayIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void connectSignals();

    void changeLanguage();
    void restart();
    void saveSettings();
    void changeTheme();

    void about();
    void credits();

    qsizetype currentIndexLanguage, currentIndexTheme;

    QWidget *titleBar;
    QToolButton *popUpButton, *minimizeButton, *closeButton;
    QLabel *appIcon, *appTitleName;
    QMenu *appMenu,
        *languagesMenu,
        *themesMenu,
        *helpMenu,
        *myTrayIconMenu;
    QList<QAction *> languageActionList, themeActionList;
    QAction *customTitleBarAction,
        *aboutAction, *creditsAction, *aboutQtAction,
        *exitAction,
        *showProgramAction;
    QSettings *userSettings;
};

#endif // WINDOW_H
