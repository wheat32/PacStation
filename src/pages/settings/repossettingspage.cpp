#include "repossettingspage.h"
#include "widgets/settingrow.h"
#include "startupchecker.h"
#include "debug.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
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
ReposSettingsPage::ReposSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadSettings();
}

// ---------------------------------------------------------------------------
void ReposSettingsPage::buildUi()
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
    connect(backBtn, &QPushButton::clicked, this, &ReposSettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Repositories"), topBar);
    pageTitle->setObjectName(QStringLiteral("settingsPageTitle"));
    topLayout->addWidget(pageTitle);
    topLayout->addStretch();
    outerLayout->addWidget(topBar);

    // ── Scrollable content ───────────────────────────────────
    auto* scroll  = new QScrollArea(this);
    auto* content = new QWidget(scroll);
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(24, 16, 24, 32);
    layout->setSpacing(8);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll, 1);

    // ── Section: Pacman Mirrors ──────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Pacman Mirrors"), content));

    auto* mirrorsRow = new SettingRow(
        tr("Mirror List"),
        tr("Edit the list of Pacman download mirrors. "
           "Mirrors are tried in order; faster mirrors reduce install times."),
        nullptr, content);
    mirrorsRow->makeNavigationRow();
    connect(mirrorsRow, &SettingRow::clicked,
            this, &ReposSettingsPage::navigateToMirrors);
    layout->addWidget(mirrorsRow);

    layout->addSpacing(16);

    // ── Section: Pacman Repositories ────────────────────────
    layout->addWidget(makeSectionHeader(tr("Pacman Repositories"), content));

    auto* repoDesc = new QLabel(
        tr("Extra repositories enabled in pacman.conf (e.g. multilib, Chaotic-AUR, "
           "CachyOS). Requires administrator privileges to apply changes."),
        content);
    repoDesc->setObjectName(QStringLiteral("settingRowSubtitle"));
    repoDesc->setWordWrap(true);
    layout->addWidget(repoDesc);

    m_pacmanReposList = new QListWidget(content);
    m_pacmanReposList->setObjectName(QStringLiteral("reposList"));
    m_pacmanReposList->setMinimumHeight(110);
    m_pacmanReposList->setFrameShape(QFrame::NoFrame);
    layout->addWidget(m_pacmanReposList);

    auto* pacmanBtnRow    = new QWidget(content);
    auto* pacmanBtnLayout = new QHBoxLayout(pacmanBtnRow);
    pacmanBtnLayout->setContentsMargins(0, 0, 0, 0);
    pacmanBtnLayout->setSpacing(8);

    auto* addRepoBtn = new QPushButton(tr("+ Add Repository"), pacmanBtnRow);
    addRepoBtn->setObjectName(QStringLiteral("settingsAddBtn"));
    addRepoBtn->setCursor(Qt::PointingHandCursor);
    connect(addRepoBtn, &QPushButton::clicked,
            this, &ReposSettingsPage::onAddPacmanRepo);
    pacmanBtnLayout->addWidget(addRepoBtn);

    auto* removeRepoBtn = new QPushButton(tr("Remove Selected"), pacmanBtnRow);
    removeRepoBtn->setObjectName(QStringLiteral("settingsRemoveBtn"));
    removeRepoBtn->setCursor(Qt::PointingHandCursor);
    connect(removeRepoBtn, &QPushButton::clicked,
            this, &ReposSettingsPage::onRemovePacmanRepo);
    pacmanBtnLayout->addWidget(removeRepoBtn);
    pacmanBtnLayout->addStretch();
    layout->addWidget(pacmanBtnRow);

    layout->addSpacing(16);

    // ── Section: AUR Helper ──────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("AUR"), content));

    const StartupChecker& checker = StartupChecker::instance();
    const QStringList helpers = checker.installedAurHelpers();

    if (helpers.isEmpty())
    {
        // Neither yay nor paru found – show a non-interactive notice row
        auto* noHelperRow = new SettingRow(
            tr("AUR Helper"),
            tr("No supported AUR helper found. Install yay or paru to enable "
               "AUR package support."),
            nullptr, content);
        noHelperRow->setEnabled(false);
        layout->addWidget(noHelperRow);
    }
    else
    {
        auto* aurCombo = new QComboBox(content);
        aurCombo->setObjectName(QStringLiteral("settingsCombo"));
        for (const QString& h : helpers)
            aurCombo->addItem(h, h);

        {
            QSettings s;
            const QString saved =
                s.value(QStringLiteral("aur/helper"), helpers.first()).toString();
            const int idx = aurCombo->findData(saved);
            aurCombo->setCurrentIndex(idx >= 0 ? idx : 0);
        }
        connect(aurCombo, &QComboBox::currentTextChanged, this, [](const QString& val)
        {
            DBG_SETTINGS(QStringLiteral("aur/helper -> ") + val);
            QSettings().setValue(QStringLiteral("aur/helper"), val);
        });

        const QString subtitle = helpers.size() > 1
            ? tr("Choose which AUR helper PacStation uses when both yay and paru "
                 "are installed.")
            : tr("Using %1 (the only supported AUR helper currently installed).")
                  .arg(helpers.first());

        auto* aurRow = new SettingRow(tr("AUR Helper"), subtitle, aurCombo, content);
        if (helpers.size() == 1)
            aurCombo->setEnabled(false); // nothing to choose – only one option
        layout->addWidget(aurRow);
    }

    layout->addSpacing(16);

    // ── Section: Flatpak Remotes ─────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Flatpak Remotes"), content));

    auto* flatpakDesc = new QLabel(
        tr("Flatpak repositories (remotes) available for browsing and installing apps. "
           "Flathub is added automatically."),
        content);
    flatpakDesc->setObjectName(QStringLiteral("settingRowSubtitle"));
    flatpakDesc->setWordWrap(true);
    layout->addWidget(flatpakDesc);

    m_flatpakRemotesList = new QListWidget(content);
    m_flatpakRemotesList->setObjectName(QStringLiteral("flatpakRemotesList"));
    m_flatpakRemotesList->setMinimumHeight(110);
    m_flatpakRemotesList->setFrameShape(QFrame::NoFrame);
    layout->addWidget(m_flatpakRemotesList);

    auto* flatpakBtnRow    = new QWidget(content);
    auto* flatpakBtnLayout = new QHBoxLayout(flatpakBtnRow);
    flatpakBtnLayout->setContentsMargins(0, 0, 0, 0);
    flatpakBtnLayout->setSpacing(8);

    auto* addRemoteBtn = new QPushButton(tr("+ Add Remote"), flatpakBtnRow);
    addRemoteBtn->setObjectName(QStringLiteral("settingsAddBtn"));
    addRemoteBtn->setCursor(Qt::PointingHandCursor);
    connect(addRemoteBtn, &QPushButton::clicked,
            this, &ReposSettingsPage::onAddFlatpakRemote);
    flatpakBtnLayout->addWidget(addRemoteBtn);

    auto* removeRemoteBtn = new QPushButton(tr("Remove Selected"), flatpakBtnRow);
    removeRemoteBtn->setObjectName(QStringLiteral("settingsRemoveBtn"));
    removeRemoteBtn->setCursor(Qt::PointingHandCursor);
    connect(removeRemoteBtn, &QPushButton::clicked,
            this, &ReposSettingsPage::onRemoveFlatpakRemote);
    flatpakBtnLayout->addWidget(removeRemoteBtn);
    flatpakBtnLayout->addStretch();
    layout->addWidget(flatpakBtnRow);

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void ReposSettingsPage::loadSettings()
{
    QSettings s;

    // Pacman repos
    const QStringList defaultRepos = { QStringLiteral("core"),
                                       QStringLiteral("extra"),
                                       QStringLiteral("multilib") };
    const QStringList repos =
        s.value(QStringLiteral("pacman/repos"), defaultRepos).toStringList();
    for (const QString& r : repos)
        m_pacmanReposList->addItem(r);

    // Flatpak remotes (default: Flathub)
    const QStringList defaultRemotes = { QStringLiteral("flathub") };
    const QStringList remotes =
        s.value(QStringLiteral("flatpak/remotes"), defaultRemotes).toStringList();
    for (const QString& r : remotes)
        m_flatpakRemotesList->addItem(r);
}

// ---------------------------------------------------------------------------
void ReposSettingsPage::savePacmanRepos()
{
    QStringList repos;
    for (int i = 0; i < m_pacmanReposList->count(); ++i)
        repos << m_pacmanReposList->item(i)->text();
    QSettings().setValue(QStringLiteral("pacman/repos"), repos);
    DBG_SETTINGS(QStringLiteral("pacman/repos saved: ") + repos.join(QStringLiteral(", ")));
}

void ReposSettingsPage::saveFlatpakRemotes()
{
    QStringList remotes;
    for (int i = 0; i < m_flatpakRemotesList->count(); ++i)
        remotes << m_flatpakRemotesList->item(i)->text();
    QSettings().setValue(QStringLiteral("flatpak/remotes"), remotes);
    DBG_SETTINGS(QStringLiteral("flatpak/remotes saved: ") + remotes.join(QStringLiteral(", ")));
}

// ---------------------------------------------------------------------------
void ReposSettingsPage::onAddPacmanRepo()
{
    bool ok = false;
    const QString name = QInputDialog::getText(
        this, tr("Add Repository"),
        tr("Repository name (as it appears in pacman.conf, e.g. multilib):"),
        QLineEdit::Normal, {}, &ok);
    if (ok && !name.trimmed().isEmpty())
    {
        m_pacmanReposList->addItem(name.trimmed());
        savePacmanRepos();
    }
}

void ReposSettingsPage::onRemovePacmanRepo()
{
    const auto selected = m_pacmanReposList->selectedItems();
    for (auto* item : selected) delete item;
    savePacmanRepos();
}

void ReposSettingsPage::onAddFlatpakRemote()
{
    auto* dlg = new QDialog(this);
    dlg->setWindowTitle(tr("Add Flatpak Remote"));
    dlg->setMinimumWidth(400);

    auto* form = new QFormLayout(dlg);
    form->setContentsMargins(20, 16, 20, 16);
    form->setSpacing(12);

    auto* nameEdit = new QLineEdit(dlg);
    nameEdit->setPlaceholderText(QStringLiteral("my-remote"));
    auto* urlEdit  = new QLineEdit(dlg);
    urlEdit->setPlaceholderText(QStringLiteral("https://example.com/repo.flatpakrepo"));

    form->addRow(tr("Remote name:"), nameEdit);
    form->addRow(tr("URL:"),         urlEdit);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dlg);
    connect(buttons, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
    form->addRow(buttons);

    if (dlg->exec() == QDialog::Accepted)
    {
        const QString name = nameEdit->text().trimmed();
        if (!name.isEmpty())
        {
            m_flatpakRemotesList->addItem(name);
            saveFlatpakRemotes();
        }
    }
    dlg->deleteLater();
}

void ReposSettingsPage::onRemoveFlatpakRemote()
{
    // Don't allow removing Flathub
    const auto selected = m_flatpakRemotesList->selectedItems();
    for (auto* item : selected)
    {
        if (item->text().toLower() != QLatin1String("flathub"))
            delete item;
    }
    saveFlatpakRemotes();
}

