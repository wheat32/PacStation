#pragma once

#include <QColor>
#include <QString>

/// Singleton that detects the system color theme and accent color,
/// and produces a ready-to-use application stylesheet.
class ThemeManager
{
public:
    static ThemeManager& instance();

    bool   isDarkMode()         const { return m_darkMode; }
    QColor accentColor()        const { return m_accentColor; }
    QColor foregroundColor()    const { return m_foreground; }
    QColor sidebarBackground()  const { return m_sidebarBg; }
    QColor contentBackground()  const { return m_contentBg; }
    QColor hoverBackground()    const { return m_hoverBg; }
    QColor accentBackground()   const { return m_accentBg; }
    QColor borderColor()        const { return m_border; }
    QColor textColor()          const { return m_foreground; }
    QColor mutedTextColor()     const { return m_mutedText; }

    /// Returns the fully substituted application stylesheet.
    QString buildStyleSheet() const;

private:
    ThemeManager();
    void   detectTheme();
    QColor readKdeAccentColor() const;

    bool   m_darkMode    = false;
    QColor m_accentColor;
    QColor m_foreground;
    QColor m_sidebarBg;
    QColor m_contentBg;
    QColor m_hoverBg;
    QColor m_accentBg;
    QColor m_border;
    QColor m_mutedText;
};
