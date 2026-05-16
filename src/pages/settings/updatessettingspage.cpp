#include "updatessettingspage.h"
#include "widgets/settingrow.h"
#include "widgets/togglewithstatus.h"
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
UpdatesSettingsPage::UpdatesSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadSettings();
}

// ---------------------------------------------------------------------------
void UpdatesSettingsPage::buildUi()
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
            this, &UpdatesSettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Updates"), topBar);
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

    // ── Section: Notifications ───────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Notifications"), content));

    m_notifyToggle = new ToggleWithStatus(nullptr);
    auto* notifyRow = new SettingRow(
        tr("Update Notifications"),
        tr("Show a desktop notification when software updates are available."),
        m_notifyToggle, content);
    layout->addWidget(notifyRow);
    connect(m_notifyToggle, &ToggleWithStatus::toggled,
            this, &UpdatesSettingsPage::onNotificationsToggled);

    // Options shown only when notifications are on
    m_notifyOptions = new QWidget(content);
    auto* optLayout = new QVBoxLayout(m_notifyOptions);
    optLayout->setContentsMargins(0, 4, 0, 0);
    optLayout->setSpacing(4);

    // Frequency
    m_frequencyCombo = new QComboBox(nullptr);
    m_frequencyCombo->setObjectName(QStringLiteral("settingsCombo"));
    m_frequencyCombo->addItem(tr("On login"),        QStringLiteral("login"));
    m_frequencyCombo->addItem(tr("Every hour"),      QStringLiteral("hourly"));
    m_frequencyCombo->addItem(tr("Every 12 hours"),  QStringLiteral("12hours"));
    m_frequencyCombo->addItem(tr("Daily"),           QStringLiteral("daily"));
    m_frequencyCombo->addItem(tr("Weekly"),          QStringLiteral("weekly"));
    connect(m_frequencyCombo, &QComboBox::currentIndexChanged, this, [this](int)
    {
        const QString val = m_frequencyCombo->currentData().toString();
        DBG_SETTINGS(QStringLiteral("updates/notifyFrequency -> ") + val);
        QSettings().setValue(QStringLiteral("updates/notifyFrequency"), val);
    });
    auto* freqRow = new SettingRow(
        tr("Check Frequency"),
        tr("How often PacStation should check for available updates."),
        m_frequencyCombo, m_notifyOptions);
    optLayout->addWidget(freqRow);

    // Per-source sub-toggles
    m_notifyPacman = new ToggleWithStatus(nullptr);
    auto* pacRow = new SettingRow(
        tr("Pacman Updates"),
        tr("Notify when official repository packages have updates."),
        m_notifyPacman, m_notifyOptions);
    optLayout->addWidget(pacRow);
    connect(m_notifyPacman, &ToggleWithStatus::toggled, this, [](bool on)
    {
        DBG_SETTINGS(QStringLiteral("updates/notifyPacman -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("updates/notifyPacman"), on);
    });

    m_notifyAur = new ToggleWithStatus(nullptr);
    auto* aurRow = new SettingRow(
        tr("AUR Updates"),
        tr("Notify when installed AUR packages have updates."),
        m_notifyAur, m_notifyOptions);
    optLayout->addWidget(aurRow);
    connect(m_notifyAur, &ToggleWithStatus::toggled, this, [](bool on)
    {
        DBG_SETTINGS(QStringLiteral("updates/notifyAur -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("updates/notifyAur"), on);
    });

    m_notifyFlatpak = new ToggleWithStatus(nullptr);
    auto* fpRow = new SettingRow(
        tr("Flatpak Updates"),
        tr("Notify when installed Flatpak apps have updates."),
        m_notifyFlatpak, m_notifyOptions);
    optLayout->addWidget(fpRow);
    connect(m_notifyFlatpak, &ToggleWithStatus::toggled, this, [](bool on)
    {
        DBG_SETTINGS(QStringLiteral("updates/notifyFlatpak -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("updates/notifyFlatpak"), on);
    });

    layout->addWidget(m_notifyOptions);
    layout->addStretch();
}

// ---------------------------------------------------------------------------
void UpdatesSettingsPage::loadSettings()
{
    QSettings s;

    const bool notify = s.value(QStringLiteral("updates/notifications"), true).toBool();
    m_notifyToggle->setOn(notify, false);
    m_notifyOptions->setVisible(notify);

    const QString freq =
        s.value(QStringLiteral("updates/notifyFrequency"),
                QStringLiteral("daily")).toString();
    for (int i = 0; i < m_frequencyCombo->count(); ++i)
        if (m_frequencyCombo->itemData(i).toString() == freq)
        { m_frequencyCombo->setCurrentIndex(i); break; }

    m_notifyPacman->setOn(
        s.value(QStringLiteral("updates/notifyPacman"),  true).toBool(), false);
    m_notifyAur->setOn(
        s.value(QStringLiteral("updates/notifyAur"),     true).toBool(), false);
    m_notifyFlatpak->setOn(
        s.value(QStringLiteral("updates/notifyFlatpak"), true).toBool(), false);
}

// ---------------------------------------------------------------------------
void UpdatesSettingsPage::onNotificationsToggled(bool on)
{
    m_notifyOptions->setVisible(on);
    DBG_SETTINGS(QStringLiteral("updates/notifications -> ") +
                 (on ? QStringLiteral("true") : QStringLiteral("false")));
    QSettings().setValue(QStringLiteral("updates/notifications"), on);
}
