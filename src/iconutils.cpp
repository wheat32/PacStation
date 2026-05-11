#include "iconutils.h"

#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

QIcon createColoredIcon(const QString& svgResourcePath,
                        const QColor& color,
                        const QSize& size)
{
    QFile file(svgResourcePath);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QByteArray svgData = file.readAll();
    svgData.replace("currentColor", color.name(QColor::HexRgb).toUtf8());

    QSvgRenderer renderer(svgData);
    if (!renderer.isValid())
        return {};

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.end();

    return QIcon(pixmap);
}

