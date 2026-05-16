#include "appearancesettingspage.h"
#include "widgets/settingrow.h"
#include "debug.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QVBoxLayout>

static QLabel* makeSectionHeader(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text.toUpper(), parent);
    label->setObjectName(QStringLiteral("settingsSectionHeader"));
    return label;
}

// ---------------------------------------------------------------------------
AppearanceSettingsPage::AppearanceSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadSettings();
}

// ---------------------------------------------------------------------------
void AppearanceSettingsPage::buildUi()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ── Top bar ──────────────────────────────────────────────
    auto* topBar    = new QWidget(this);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 12, 16, 12);
    topLayout->setSpacing(8);

    auto* backBtn = new QPushButton(tr("‹  Back"), topBar);
    backBtn->setObjectName(QStringLiteral("settingsBackBtn"));
    backBtn->setFlat(true);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked,
            this, &AppearanceSettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Appearance"), topBar);
    pageTitle->setObjectName(QStringLiteral("settingsPageTitle"));
    topLayout->addWidget(pageTitle);
    topLayout->addStretch();
    outerLayout->addWidget(topBar);

    // ── Content ───────────────────────────────────────────────
    auto* scroll  = new QScrollArea(this);
    auto* content = new QWidget(scroll);
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(24, 16, 24, 32);
    layout->setSpacing(8);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll, 1);

    layout->addWidget(makeSectionHeader(tr("Theme"), content));

    m_themeCombo = new QComboBox(nullptr);
    m_themeCombo->setObjectName(QStringLiteral("settingsCombo"));
    m_themeCombo->addItem(tr("Follow System"), QStringLiteral("system"));
    m_themeCombo->addItem(tr("Light"),         QStringLiteral("light"));
    m_themeCombo->addItem(tr("Dark"),          QStringLiteral("dark"));
    connect(m_themeCombo, &QComboBox::currentIndexChanged, this, [this](int)
    {
        const QString val = m_themeCombo->currentData().toString();
        DBG_SETTINGS(QStringLiteral("appearance/theme -> ") + val);
        QSettings().setValue(QStringLiteral("appearance/theme"), val);
    });

    auto* themeRow = new SettingRow(
        tr("Color Theme"),
        tr("Choose whether PacStation follows your system theme or uses a fixed "
           "light or dark appearance."),
        m_themeCombo, content);
    layout->addWidget(themeRow);

    layout->addSpacing(16);
    layout->addWidget(makeSectionHeader(tr("Layout"), content));

    m_densityCombo = new QComboBox(nullptr);
    m_densityCombo->setObjectName(QStringLiteral("settingsCombo"));
    m_densityCombo->addItem(tr("Comfortable"), QStringLiteral("comfortable"));
    m_densityCombo->addItem(tr("Compact"),     QStringLiteral("compact"));
    connect(m_densityCombo, &QComboBox::currentIndexChanged, this, [this](int)
    {
        const QString val = m_densityCombo->currentData().toString();
        DBG_SETTINGS(QStringLiteral("appearance/density -> ") + val);
        QSettings().setValue(QStringLiteral("appearance/density"), val);
    });

    auto* densityRow = new SettingRow(
        tr("List Density"),
        tr("Controls the amount of vertical padding in package lists."),
        m_densityCombo, content);
    layout->addWidget(densityRow);

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void AppearanceSettingsPage::loadSettings()
{
    QSettings s;

    const QString theme =
        s.value(QStringLiteral("appearance/theme"), QStringLiteral("system")).toString();
    for (int i = 0; i < m_themeCombo->count(); ++i)
        if (m_themeCombo->itemData(i).toString() == theme)
        { m_themeCombo->setCurrentIndex(i); break; }

    const QString density =
        s.value(QStringLiteral("appearance/density"),
                QStringLiteral("comfortable")).toString();
    for (int i = 0; i < m_densityCombo->count(); ++i)
        if (m_densityCombo->itemData(i).toString() == density)
        { m_densityCombo->setCurrentIndex(i); break; }
}

