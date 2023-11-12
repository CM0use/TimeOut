// TimeOut
// Copyright (c) 2023 CM0use

#include "clock.h"
#include "icons.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTranslator>
#include <QMessageBox>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app{ argc, argv };
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setApplicationName("TimeOut");
    app.setWindowIcon(APP_ICON_LIGHT);
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CM0use");
    app.setOrganizationDomain("https://github.com/CM0use");

    if (!QFile::exists("config")) {
        const QDir config{ app.applicationDirPath() };
        config.mkdir("config");
    }

    QSettings settings{ app.applicationDirPath() + "/config/Settings.ini",
                       QSettings::Format::IniFormat };
    if (!QFile::exists(settings.fileName())) {
        settings.setValue("Settings/IndexLanguage", 0);
        settings.setValue("Settings/IndexTheme", 0);
    }

    QTranslator translator;
    const QStringList languages{ "en.qm", "es.qm" };
    int indexLanguage{ settings.value("Settings/IndexLanguage").toInt() };
    if (indexLanguage >= 0 && indexLanguage < languages.size()) {
        if (!indexLanguage);
        else if (translator.load(":/i18n/" + languages[indexLanguage]))
            app.installTranslator(&translator);
        else QMessageBox::warning(nullptr, "Warning",
                                 "Could not load translation file.",
                                 QMessageBox::StandardButton::Close);
    } else return QMessageBox::critical(nullptr, "Error",
                                     "The language index specified in the "
                                     "configuration file is invalid. \n"
                                     "Please check the value and try again.",
                                     QMessageBox::StandardButton::Abort);

    const QStringList themes{ "light.css", "dark.css" };
    int indexTheme{ settings.value("Settings/IndexTheme").toInt() };
    if (indexTheme >= 0 && indexTheme < themes.size()) {
        QFile styleSheetFile{ ":/qss/" + themes[indexTheme] };
        styleSheetFile.open(QFile::ReadOnly);
        app.setStyleSheet(styleSheetFile.readAll());
    } else return QMessageBox::critical(nullptr, QObject::tr("Error"),
                                     QObject::tr("The theme index specified "
                                                 "in the configuration file "
                                                 "is invalid. \nPlease check "
                                                 "the value and try again."),
                                     QMessageBox::StandardButton::Abort);

    TimeOut mainWindow;
    mainWindow.setFixedHeight(mainWindow.sizeHint().height());
    mainWindow.show();

    return app.exec();
}
