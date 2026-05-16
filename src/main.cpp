#include "mainwindow.h"
#include "thememanager.h"
#include "debug.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTranslator>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("PacStation"));
    app.setApplicationDisplayName(QStringLiteral("PacStation"));
    app.setOrganizationName(QStringLiteral("PacStation"));

    // ── Read version from embedded version.json ───────────────
    QString appVersion = QStringLiteral("unknown");
    {
        QFile f(QStringLiteral(":/version.json"));
        if (f.open(QIODevice::ReadOnly))
        {
            const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            appVersion = doc.object()
                             .value(QStringLiteral("app_version"))
                             .toString(QStringLiteral("unknown"));
        }
    }

    // ── Startup diagnostics ───────────────────────────────────
    DBG_APP(QStringLiteral("=== PacStation starting ==="));
    DBG_APP(QStringLiteral("App version : ") + appVersion);
    DBG_APP(QStringLiteral("Qt version  : ") + QString::fromLatin1(qVersion()));
    DBG_APP(QStringLiteral("OS          : ") + QSysInfo::prettyProductName());
    DBG_APP(QStringLiteral("Kernel      : ") + QSysInfo::kernelVersion());
    DBG_APP(QStringLiteral("CPU arch    : ") + QSysInfo::currentCpuArchitecture());
    DBG_APP(QStringLiteral("Locale      : ") + QLocale::system().name());
    DBG_APP(QStringLiteral("Config dir  : ") +
            QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    DBG_APP(QStringLiteral("==========================="));

    // ── Translations ─────────────────────────────────────────
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages)
    {
        const QString name = QStringLiteral("pacstation_") + QLocale(locale).name();
        if (translator.load(QStringLiteral(":/i18n/") + name))
        {
            app.installTranslator(&translator);
            DBG_APP(QStringLiteral("Loaded translation: ") + name);
            break;
        }
    }

    // ── Stylesheet ───────────────────────────────────────────
    const QString styleSheet = ThemeManager::instance().buildStyleSheet();
    app.setStyleSheet(styleSheet);
    DBG_APP(QStringLiteral("Theme: ") +
            (ThemeManager::instance().isDarkMode()
                 ? QStringLiteral("dark")
                 : QStringLiteral("light")));

    MainWindow window;
    window.show();

    return QApplication::exec();
}
