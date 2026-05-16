#pragma once

#include <QColor>
#include <QString>
#include <QToolButton>

/// A checkable sidebar navigation button.
/// Displays a large icon above a short label, centered – similar to the
/// Microsoft Store sidebar style. Switching from normal to checked swaps
/// to the "filled" icon variant and applies the system accent color.
class SidebarButton : public QToolButton
{
    Q_OBJECT

public:
    SidebarButton(const QString& label,
                  const QString& normalSvgPath,
                  const QString& filledSvgPath,
                  QWidget* parent = nullptr);

    /// Call after the ThemeManager is ready (or on theme change).
    void applyTheme(const QColor& foreground, const QColor& accent);

private slots:
    void onToggled(bool checked);

private:
    QString m_normalPath;
    QString m_filledPath;
    QColor  m_foreground;
    QColor  m_accent;

    void refreshIcon(bool checked);
};
