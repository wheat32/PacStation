#include "searchsettingspage.h"
#include "widgets/settingrow.h"
#include "widgets/togglewithstatus.h"
#include "debug.h"

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
SearchSettingsPage::SearchSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadSettings();
}

// ---------------------------------------------------------------------------
void SearchSettingsPage::buildUi()
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
            this, &SearchSettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Search"), topBar);
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

    layout->addWidget(makeSectionHeader(tr("Included Sources"), content));

    // AUR toggle
    m_includeAur = new ToggleWithStatus(nullptr);
    auto* aurRow = new SettingRow(
        tr("Include AUR in Search"),
        tr("Show AUR (Arch User Repository) results alongside official packages. "
           "AUR packages are community-maintained and not officially supported."),
        m_includeAur, content);
    layout->addWidget(aurRow);
    connect(m_includeAur, &ToggleWithStatus::toggled, this, [](bool on)
    {
        DBG_SETTINGS(QStringLiteral("search/includeAur -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("search/includeAur"), on);
    });

    // Flatpak toggle
    m_includeFlatpak = new ToggleWithStatus(nullptr);
    auto* fpRow = new SettingRow(
        tr("Include Flatpak in Search"),
        tr("Show results from configured Flatpak remotes (e.g. Flathub)."),
        m_includeFlatpak, content);
    layout->addWidget(fpRow);
    connect(m_includeFlatpak, &ToggleWithStatus::toggled, this, [this](bool on)
    {
        DBG_SETTINGS(QStringLiteral("search/includeFlatpak -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("search/includeFlatpak"), on);
        m_allRemotes->parentWidget()->setEnabled(on);
    });

    // All remotes toggle
    m_allRemotes = new ToggleWithStatus(nullptr);
    auto* remoteRow = new SettingRow(
        tr("Search All Flatpak Remotes"),
        tr("When off, only Flathub results are shown. When on, results from all "
           "configured remotes are included."),
        m_allRemotes, content);
    layout->addWidget(remoteRow);
    connect(m_allRemotes, &ToggleWithStatus::toggled, this, [](bool on)
    {
        DBG_SETTINGS(QStringLiteral("search/allFlatpakRemotes -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("search/allFlatpakRemotes"), on);
    });

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void SearchSettingsPage::loadSettings()
{
    QSettings s;
    m_includeAur->setOn(
        s.value(QStringLiteral("search/includeAur"),          true).toBool(), false);
    m_includeFlatpak->setOn(
        s.value(QStringLiteral("search/includeFlatpak"),      true).toBool(), false);
    m_allRemotes->setOn(
        s.value(QStringLiteral("search/allFlatpakRemotes"),  false).toBool(), false);
}

