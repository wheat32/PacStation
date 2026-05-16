#include "thememanager.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager()
{
    detectTheme();
}

// ---------------------------------------------------------------------------
// Attempt to read the accent color from KDE's kdeglobals config.
// Returns an invalid QColor when the file or key cannot be found.
// ---------------------------------------------------------------------------
QColor ThemeManager::readKdeAccentColor() const
{
    const QString path = QStandardPaths::locate(
        QStandardPaths::GenericConfigLocation, QStringLiteral("kdeglobals"));
    if (path.isEmpty())
        return {};

    QSettings kde(path, QSettings::IniFormat);

    // KDE 5/6 stores the accent under [General] → AccentColor (newer builds)
    // or under [Colors:Selection] → BackgroundNormal (classic palette).
    kde.beginGroup(QStringLiteral("General"));
    const QString accentStr = kde.value(QStringLiteral("AccentColor")).toString();
    kde.endGroup();

    if (!accentStr.isEmpty())
    {
        const QStringList parts = accentStr.split(QLatin1Char(','));
        if (parts.size() == 3)
        {
            bool ok1, ok2, ok3;
            return QColor(parts[0].trimmed().toInt(&ok1),
                          parts[1].trimmed().toInt(&ok2),
                          parts[2].trimmed().toInt(&ok3));
        }
    }

    kde.beginGroup(QStringLiteral("Colors:Selection"));
    const QString bgStr = kde.value(QStringLiteral("BackgroundNormal")).toString();
    kde.endGroup();

    if (!bgStr.isEmpty())
    {
        const QStringList parts = bgStr.split(QLatin1Char(','));
        if (parts.size() == 3)
        {
            bool ok1, ok2, ok3;
            return QColor(parts[0].trimmed().toInt(&ok1),
                          parts[1].trimmed().toInt(&ok2),
                          parts[2].trimmed().toInt(&ok3));
        }
    }

    return {};
}

void ThemeManager::detectTheme()
{
    const QPalette pal    = QApplication::palette();
    const QColor windowColor = pal.color(QPalette::Window);
    m_darkMode = windowColor.lightness() < 128;

    // --- Accent color ---
    const QColor kdeAccent = readKdeAccentColor();
    if (kdeAccent.isValid())
        m_accentColor = kdeAccent;
    else
        m_accentColor = pal.color(QPalette::Highlight);

    // --- Derived palette ---
    m_foreground = pal.color(QPalette::WindowText);

    if (m_darkMode)
    {
        m_sidebarBg = windowColor.lighter(115);
        m_contentBg = windowColor;
        m_hoverBg   = QColor(255, 255, 255, 18);
        m_accentBg  = QColor(m_accentColor.red(),
                             m_accentColor.green(),
                             m_accentColor.blue(), 45);
        m_border    = QColor(255, 255, 255, 20);
        m_mutedText = QColor(255, 255, 255, 120);
    }
    else
    {
        m_sidebarBg = windowColor.darker(105);
        m_contentBg = windowColor;
        m_hoverBg   = QColor(0, 0, 0, 14);
        m_accentBg  = QColor(m_accentColor.red(),
                             m_accentColor.green(),
                             m_accentColor.blue(), 40);
        m_border    = QColor(0, 0, 0, 20);
        m_mutedText = QColor(0, 0, 0, 120);
    }
}

// ---------------------------------------------------------------------------
// Load style.qss from resources and replace color tokens.
// ---------------------------------------------------------------------------
QString ThemeManager::buildStyleSheet() const
{
    QFile f(QStringLiteral(":/style.qss"));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QString css = QTextStream(&f).readAll();

    auto hex = [](const QColor& c) -> QString
    {
        return c.name(QColor::HexRgb);
    };
    auto rgba = [](const QColor& c) -> QString
    {
        return QStringLiteral("rgba(%1,%2,%3,%4)")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
    };

    css.replace(QLatin1String("%ACCENT_COLOR%"), hex(m_accentColor));
    css.replace(QLatin1String("%ACCENT_BG%"),    rgba(m_accentBg));
    css.replace(QLatin1String("%SIDEBAR_BG%"),   hex(m_sidebarBg));
    css.replace(QLatin1String("%CONTENT_BG%"),   hex(m_contentBg));
    css.replace(QLatin1String("%HOVER_BG%"),     rgba(m_hoverBg));
    css.replace(QLatin1String("%BORDER_COLOR%"), rgba(m_border));
    css.replace(QLatin1String("%TEXT_COLOR%"),   hex(m_foreground));
    css.replace(QLatin1String("%MUTED_TEXT%"),   rgba(m_mutedText));


    return css;
}
