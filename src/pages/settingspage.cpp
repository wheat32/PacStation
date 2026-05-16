#include "settingspage.h"
#include "settings/sourcessettingspage.h"
#include "settings/repossettingspage.h"
#include "settings/mirrorssettingspage.h"
#include "settings/updatessettingspage.h"
#include "settings/searchsettingspage.h"
#include "settings/appearancesettingspage.h"
#include "settings/securitysettingspage.h"
#include "widgets/settingrow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
SettingsPage::SettingsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    rootLayout->addWidget(m_stack);

    // Hub (index 0) – built inline; never removed from the stack
    auto* hub = new QWidget(m_stack);
    m_stack->addWidget(hub);
    buildHub(hub);
    m_history.push(hub);
}

// ---------------------------------------------------------------------------
// Small helper reused by the hub to group rows under a labelled section.
// ---------------------------------------------------------------------------
static QLabel* makeSectionHeader(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text.toUpper(), parent);
    label->setObjectName(QStringLiteral("settingsSectionHeader"));
    return label;
}

// ---------------------------------------------------------------------------
void SettingsPage::buildHub(QWidget* hub)
{
    auto* outerLayout = new QVBoxLayout(hub);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ── Page title bar ───────────────────────────────────────
    auto* titleBar    = new QWidget(hub);
    auto* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(24, 20, 24, 8);

    auto* title = new QLabel(tr("Settings"), titleBar);
    title->setObjectName(QStringLiteral("settingsHubTitle"));
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    outerLayout->addWidget(titleBar);

    // ── Scrollable list of categories ────────────────────────
    auto* scroll  = new QScrollArea(hub);
    auto* content = new QWidget(scroll);
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(24, 8, 24, 32);
    layout->setSpacing(8);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll, 1);

    // ── Sources ───────────────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Sources"), content));

    auto* sourcesRow = new SettingRow(
        tr("Source Priority & Overrides"),
        tr("Set the preferred order of package sources (Pacman, AUR, Flatpak) "
           "and configure per-app source pins."),
        nullptr, content);
    sourcesRow->makeNavigationRow();
    connect(sourcesRow, &SettingRow::clicked, this, [this]
    {
        if (!m_sourcesPage)
        {
            m_sourcesPage = new SourcesSettingsPage(m_stack);
            m_stack->addWidget(m_sourcesPage);
            connect(m_sourcesPage, &SourcesSettingsPage::backRequested,
                    this, &SettingsPage::navigateBack);
        }
        navigateTo(m_sourcesPage);
    });
    layout->addWidget(sourcesRow);

    layout->addSpacing(12);

    // ── Repositories ─────────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Repositories"), content));

    auto* reposRow = new SettingRow(
        tr("Repositories & Mirrors"),
        tr("Manage Pacman mirrors, extra repositories, AUR helper, and Flatpak remotes."),
        nullptr, content);
    reposRow->makeNavigationRow();
    connect(reposRow, &SettingRow::clicked, this, [this]
    {
        if (!m_reposPage)
        {
            m_reposPage = new ReposSettingsPage(m_stack);
            m_stack->addWidget(m_reposPage);
            connect(m_reposPage, &ReposSettingsPage::backRequested,
                    this, &SettingsPage::navigateBack);
            connect(m_reposPage, &ReposSettingsPage::navigateToMirrors, this, [this]
            {
                if (!m_mirrorsPage)
                {
                    m_mirrorsPage = new MirrorsSettingsPage(m_stack);
                    m_stack->addWidget(m_mirrorsPage);
                    connect(m_mirrorsPage, &MirrorsSettingsPage::backRequested,
                            this, &SettingsPage::navigateBack);
                }
                navigateTo(m_mirrorsPage);
            });
        }
        navigateTo(m_reposPage);
    });
    layout->addWidget(reposRow);

    layout->addSpacing(12);

    // ── Updates ──────────────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Updates"), content));

    auto* updatesRow = new SettingRow(
        tr("Update Notifications"),
        tr("Configure when and how PacStation notifies you about available updates."),
        nullptr, content);
    updatesRow->makeNavigationRow();
    connect(updatesRow, &SettingRow::clicked, this, [this]
    {
        if (!m_updatesPage)
        {
            m_updatesPage = new UpdatesSettingsPage(m_stack);
            m_stack->addWidget(m_updatesPage);
            connect(m_updatesPage, &UpdatesSettingsPage::backRequested,
                    this, &SettingsPage::navigateBack);
        }
        navigateTo(m_updatesPage);
    });
    layout->addWidget(updatesRow);

    layout->addSpacing(12);

    // ── Search ───────────────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Search"), content));

    auto* searchRow = new SettingRow(
        tr("Search Behavior"),
        tr("Choose which sources appear in search results."),
        nullptr, content);
    searchRow->makeNavigationRow();
    connect(searchRow, &SettingRow::clicked, this, [this]
    {
        if (!m_searchPage)
        {
            m_searchPage = new SearchSettingsPage(m_stack);
            m_stack->addWidget(m_searchPage);
            connect(m_searchPage, &SearchSettingsPage::backRequested,
                    this, &SettingsPage::navigateBack);
        }
        navigateTo(m_searchPage);
    });
    layout->addWidget(searchRow);

    layout->addSpacing(12);

    // ── Appearance ───────────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Appearance"), content));

    auto* appearRow = new SettingRow(
        tr("Theme & Layout"),
        tr("Switch between light and dark themes and adjust list density."),
        nullptr, content);
    appearRow->makeNavigationRow();
    connect(appearRow, &SettingRow::clicked, this, [this]
    {
        if (!m_appearancePage)
        {
            m_appearancePage = new AppearanceSettingsPage(m_stack);
            m_stack->addWidget(m_appearancePage);
            connect(m_appearancePage, &AppearanceSettingsPage::backRequested,
                    this, &SettingsPage::navigateBack);
        }
        navigateTo(m_appearancePage);
    });
    layout->addWidget(appearRow);

    layout->addSpacing(12);

    // ── Security ─────────────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Security & Trust"), content));

    auto* secRow = new SettingRow(
        tr("Security & Cache"),
        tr("Configure AUR installation warnings and clean up cached package data."),
        nullptr, content);
    secRow->makeNavigationRow();
    connect(secRow, &SettingRow::clicked, this, [this]
    {
        if (!m_securityPage)
        {
            m_securityPage = new SecuritySettingsPage(m_stack);
            m_stack->addWidget(m_securityPage);
            connect(m_securityPage, &SecuritySettingsPage::backRequested,
                    this, &SettingsPage::navigateBack);
        }
        navigateTo(m_securityPage);
    });
    layout->addWidget(secRow);

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void SettingsPage::navigateToHub()
{
    m_history.clear();
    QWidget* hub = m_stack->widget(0);
    m_history.push(hub);
    m_stack->setCurrentWidget(hub);
}

// ---------------------------------------------------------------------------
void SettingsPage::navigateTo(QWidget* page)
{
    m_history.push(page);
    m_stack->setCurrentWidget(page);
}

// ---------------------------------------------------------------------------
void SettingsPage::navigateBack()
{
    if (m_history.size() <= 1)
        return;
    m_history.pop();
    m_stack->setCurrentWidget(m_history.top());
}
