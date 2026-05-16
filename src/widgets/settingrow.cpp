#include "settingrow.h"

#include <QMouseEvent>
#include <QVBoxLayout>

SettingRow::SettingRow(const QString& title,
                       const QString& subtitle,
                       QWidget*       control,
                       QWidget*       parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("settingRow"));
    setAttribute(Qt::WA_Hover);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(16, 14, 16, 14);
    m_mainLayout->setSpacing(14);

    // ── Icon placeholder (36×36) ─────────────────────────────
    auto* iconHolder = new QWidget(this);
    iconHolder->setObjectName(QStringLiteral("settingRowIcon"));
    iconHolder->setFixedSize(36, 36);
    m_mainLayout->addWidget(iconHolder, 0, Qt::AlignVCenter);

    // ── Text block ───────────────────────────────────────────
    auto* textBlock  = new QWidget(this);
    auto* textLayout = new QVBoxLayout(textBlock);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);

    m_titleLabel = new QLabel(title, textBlock);
    m_titleLabel->setObjectName(QStringLiteral("settingRowTitle"));
    textLayout->addWidget(m_titleLabel);

    if (!subtitle.isEmpty())
    {
        m_subtitleLabel = new QLabel(subtitle, textBlock);
        m_subtitleLabel->setObjectName(QStringLiteral("settingRowSubtitle"));
        m_subtitleLabel->setWordWrap(true);
        textLayout->addWidget(m_subtitleLabel);
    }

    m_mainLayout->addWidget(textBlock, 1, Qt::AlignVCenter);

    if (control)
        setControl(control);
}

// ---------------------------------------------------------------------------
void SettingRow::setControl(QWidget* control)
{
    if (m_control)
    {
        m_mainLayout->removeWidget(m_control);
        m_control->setParent(nullptr);
    }
    m_control = control;
    if (m_control)
    {
        m_control->setParent(this);
        m_mainLayout->addWidget(m_control, 0, Qt::AlignVCenter);
    }
}

// ---------------------------------------------------------------------------
void SettingRow::makeNavigationRow()
{
    m_isNavRow = true;
    setCursor(Qt::PointingHandCursor);
    auto* chevron = new QLabel(QStringLiteral("›"), this);
    chevron->setObjectName(QStringLiteral("settingRowChevron"));
    setControl(chevron);
}

// ---------------------------------------------------------------------------
void SettingRow::mouseReleaseEvent(QMouseEvent* e)
{
    if (m_isNavRow && e->button() == Qt::LeftButton && rect().contains(e->pos()))
        emit clicked();
    QFrame::mouseReleaseEvent(e);
}

