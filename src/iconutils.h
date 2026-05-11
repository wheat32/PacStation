#pragma once

#include <QIcon>
#include <QColor>
#include <QSize>
#include <QString>

/// Loads an SVG from a Qt resource path, replaces "currentColor" with @color,
/// and returns a QIcon rendered at @size.
QIcon createColoredIcon(const QString& svgResourcePath,
                        const QColor& color,
                        const QSize& size = QSize(24, 24));

