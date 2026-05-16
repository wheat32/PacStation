#include "sidebarbutton.h"
#include "iconutils.h"

#include <QSizePolicy>

SidebarButton::SidebarButton(const QString& label,
                             const QString& normalSvgPath,
                             const QString& filledSvgPath,
                             QWidget* parent)
    : QToolButton(parent)
    , m_normalPath(normalSvgPath)
    , m_filledPath(filledSvgPath)
{
    setText(label);
    setCheckable(true);
    setAutoExclusive(true);
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(QSize(26, 26));
    setFixedWidth(80);
    setMinimumHeight(64);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFocusPolicy(Qt::NoFocus);

    connect(this, &QToolButton::toggled, this, &SidebarButton::onToggled);
}

void SidebarButton::applyTheme(const QColor& foreground, const QColor& accent)
{
    m_foreground = foreground;
    m_accent     = accent;
    refreshIcon(isChecked());
}

void SidebarButton::onToggled(const bool checked)
{
    refreshIcon(checked);
    update();
}

void SidebarButton::refreshIcon(const bool checked)
{
    if (checked && !m_filledPath.isEmpty())
        setIcon(createColoredIcon(m_filledPath, m_accent, iconSize()));
    else
        setIcon(createColoredIcon(m_normalPath, m_foreground, iconSize()));
}
