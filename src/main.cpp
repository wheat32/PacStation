#include "mainwindow.h"
#include "thememanager.h"

#include <QApplication>
#include <QFile>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("PacStation"));
    app.setApplicationDisplayName(QStringLiteral("PacStation"));

    // Build and apply stylesheet (must happen after QApplication exists so
    // the palette – and therefore theme detection – is ready).
    const QString styleSheet = ThemeManager::instance().buildStyleSheet();
    app.setStyleSheet(styleSheet);

    MainWindow window;
    window.show();

    return QApplication::exec();
}
