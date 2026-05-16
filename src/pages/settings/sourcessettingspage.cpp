#include "sourcessettingspage.h"
#include "widgets/settingrow.h"
#include "widgets/sourceprioritylist.h"
#include "widgets/togglewithstatus.h"
#include "widgets/infobanner.h"
#include "startupchecker.h"
#include "debug.h"

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
#include <QStringList>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Small helper: builds a section-header label (muted, uppercase, small font).
// ---------------------------------------------------------------------------
static QLabel* makeSectionHeader(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text.toUpper(), parent);
    label->setObjectName(QStringLiteral("settingsSectionHeader"));
    return label;
}

// ---------------------------------------------------------------------------
SourcesSettingsPage::SourcesSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadSettings();
}

// ---------------------------------------------------------------------------
void SourcesSettingsPage::buildUi()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ── Top bar: back button + page title ────────────────────
    auto* topBar    = new QWidget(this);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 12, 16, 12);
    topLayout->setSpacing(8);

    auto* backBtn = new QPushButton(tr("‹  Back"), topBar);
    backBtn->setObjectName(QStringLiteral("settingsBackBtn"));
    backBtn->setFlat(true);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &SourcesSettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Sources"), topBar);
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

    // ── Section 1: Source Priority ───────────────────────────
    layout->addWidget(makeSectionHeader(tr("Source Priority"), content));

    auto* priorityDesc = new QLabel(
        tr("Drag sources into your preferred order. When an app is available from "
           "multiple sources, the one highest on this list will be shown first. "
           "Disable a source to hide it from search results entirely."),
        content);
    priorityDesc->setObjectName(QStringLiteral("settingRowSubtitle"));
    priorityDesc->setWordWrap(true);
    layout->addWidget(priorityDesc);

    m_priorityList = new SourcePriorityList(content);
    layout->addWidget(m_priorityList);
    connect(m_priorityList, &SourcePriorityList::sourcesChanged,
            this, &SourcesSettingsPage::saveSourceOrder);
    layout->addSpacing(20);

    // ── Section 2: Developer Recommendations ────────────────
    layout->addWidget(makeSectionHeader(tr("Developer Recommendations"), content));

    m_honorToggle = new ToggleWithStatus(nullptr);
    auto* honorRow = new SettingRow(
        tr("Honor App Source Preferences"),
        tr("Some developers distribute their app exclusively through a specific source "
           "(e.g. OBS Studio via Flathub). When enabled, PacStation will always suggest "
           "that source first for those apps."),
        m_honorToggle, content);
    layout->addWidget(honorRow);
    connect(m_honorToggle, &ToggleWithStatus::toggled,
            this, &SourcesSettingsPage::onHonorToggled);

    // ── Per-app overrides (shown only when honor toggle is on) ─
    m_overridesSection = new QWidget(content);
    auto* overLayout   = new QVBoxLayout(m_overridesSection);
    overLayout->setContentsMargins(0, 4, 0, 0);
    overLayout->setSpacing(6);

    auto* overDesc = new QLabel(
        tr("These overrides pin a specific app to a specific source regardless of your "
           "priority list. App IDs use reverse-DNS format (e.g. com.obsproject.Studio)."),
        m_overridesSection);
    overDesc->setObjectName(QStringLiteral("settingRowSubtitle"));
    overDesc->setWordWrap(true);
    overLayout->addWidget(overDesc);

    m_overridesList = new QListWidget(m_overridesSection);
    m_overridesList->setObjectName(QStringLiteral("overridesList"));
    m_overridesList->setMinimumHeight(120);
    m_overridesList->setFrameShape(QFrame::NoFrame);
    overLayout->addWidget(m_overridesList);

    auto* btnRow    = new QWidget(m_overridesSection);
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(8);

    auto* addBtn = new QPushButton(tr("+ Add Override"), btnRow);
    addBtn->setObjectName(QStringLiteral("settingsAddBtn"));
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &SourcesSettingsPage::onAddOverride);
    btnLayout->addWidget(addBtn);

    auto* removeBtn = new QPushButton(tr("Remove Selected"), btnRow);
    removeBtn->setObjectName(QStringLiteral("settingsRemoveBtn"));
    removeBtn->setCursor(Qt::PointingHandCursor);
    connect(removeBtn, &QPushButton::clicked, this, &SourcesSettingsPage::onRemoveOverride);
    btnLayout->addWidget(removeBtn);
    btnLayout->addStretch();

    overLayout->addWidget(btnRow);
    layout->addWidget(m_overridesSection);

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void SourcesSettingsPage::loadSettings()
{
    QSettings s;
    s.beginGroup(QStringLiteral("sources"));

    // Source priority order & enabled state
    const QStringList defaultOrder = {
        QStringLiteral("flathub"),
        QStringLiteral("pacman"),
        QStringLiteral("aur")
    };
    const QStringList order = s.value(QStringLiteral("order"), defaultOrder).toStringList();

    // Friendly display names (not stored – always use tr())
    const QMap<QString, QString> names = {
        { QStringLiteral("flathub"), tr("Flathub (Flatpak)") },
        { QStringLiteral("pacman"),  tr("Official Repositories (Pacman)") },
        { QStringLiteral("aur"),     tr("AUR (Arch User Repository)") },
    };

    QList<SourceEntry> entries;
    for (const QString& id : order)
    {
        const bool enabled = s.value(QStringLiteral("enabled/") + id, true).toBool();
        entries.append({ id, names.value(id, id), enabled });
    }
    m_priorityList->setSources(entries);

    // Disable the AUR row if no supported AUR helper is installed
    if (!StartupChecker::instance().hasAnyAurHelper())
        m_priorityList->setSourceDisabled(QStringLiteral("aur"), true);

    // Honor toggle
    const bool honor = s.value(QStringLiteral("honorPreferences"), true).toBool();
    m_honorToggle->setOn(honor, /*animate=*/false);
    m_overridesSection->setVisible(honor);

    // Per-app overrides
    const int count = s.beginReadArray(QStringLiteral("overrides"));
    for (int i = 0; i < count; ++i)
    {
        s.setArrayIndex(i);
        const QString appId  = s.value(QStringLiteral("appId")).toString();
        const QString source = s.value(QStringLiteral("source")).toString();
        m_overridesList->addItem(QStringLiteral("%1  →  %2").arg(appId, source));
        m_overridesList->item(m_overridesList->count() - 1)
            ->setData(Qt::UserRole,     appId);
        m_overridesList->item(m_overridesList->count() - 1)
            ->setData(Qt::UserRole + 1, source);
    }
    s.endArray();

    s.endGroup();
}

// ---------------------------------------------------------------------------
void SourcesSettingsPage::saveSourceOrder()
{
    QSettings s;
    s.beginGroup(QStringLiteral("sources"));
    const QList<SourceEntry> entries = m_priorityList->sources();
    QStringList order;
    for (const SourceEntry& e : entries)
    {
        order << e.id;
        s.setValue(QStringLiteral("enabled/") + e.id, e.enabled);
    }
    s.setValue(QStringLiteral("order"), order);
    s.endGroup();
    DBG_SETTINGS(QStringLiteral("Source order saved: ") + order.join(QStringLiteral(", ")));
}

// ---------------------------------------------------------------------------
void SourcesSettingsPage::onHonorToggled(bool on)
{
    m_overridesSection->setVisible(on);
    QSettings().setValue(QStringLiteral("sources/honorPreferences"), on);
    DBG_SETTINGS(QStringLiteral("sources/honorPreferences -> ") +
                 (on ? QStringLiteral("true") : QStringLiteral("false")));
}

// ---------------------------------------------------------------------------
void SourcesSettingsPage::onAddOverride()
{
    // Simple two-field dialog: app ID + source
    auto* dlg = new QDialog(this);
    dlg->setWindowTitle(tr("Add Source Override"));
    dlg->setMinimumWidth(380);

    auto* form = new QFormLayout(dlg);
    form->setContentsMargins(20, 16, 20, 16);
    form->setSpacing(12);

    auto* appIdEdit  = new QLineEdit(dlg);
    appIdEdit->setPlaceholderText(QStringLiteral("com.example.App"));
    auto* sourceEdit = new QLineEdit(dlg);
    sourceEdit->setPlaceholderText(QStringLiteral("flathub"));

    form->addRow(tr("App ID:"),  appIdEdit);
    form->addRow(tr("Source:"),  sourceEdit);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dlg);
    connect(buttons, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
    form->addRow(buttons);

    if (dlg->exec() == QDialog::Accepted)
    {
        const QString appId  = appIdEdit->text().trimmed();
        const QString source = sourceEdit->text().trimmed();
        if (appId.isEmpty() || source.isEmpty())
            return;

        DBG_SETTINGS(QStringLiteral("Source override added: %1 -> %2").arg(appId, source));

        auto* item = new QListWidgetItem(
            QStringLiteral("%1  →  %2").arg(appId, source));
        item->setData(Qt::UserRole,     appId);
        item->setData(Qt::UserRole + 1, source);
        m_overridesList->addItem(item);

        QSettings s;
        s.beginGroup(QStringLiteral("sources"));
        const int count = m_overridesList->count();
        s.beginWriteArray(QStringLiteral("overrides"), count);
        for (int i = 0; i < count; ++i)
        {
            s.setArrayIndex(i);
            s.setValue(QStringLiteral("appId"),
                       m_overridesList->item(i)->data(Qt::UserRole));
            s.setValue(QStringLiteral("source"),
                       m_overridesList->item(i)->data(Qt::UserRole + 1));
        }
        s.endArray();
        s.endGroup();
    }
    dlg->deleteLater();
}

// ---------------------------------------------------------------------------
void SourcesSettingsPage::onRemoveOverride()
{
    const auto selected = m_overridesList->selectedItems();
    for (auto* item : selected)
    {
        DBG_SETTINGS(QStringLiteral("Source override removed: ") +
                     item->data(Qt::UserRole).toString());
        delete item;
    }

    QSettings s;
    s.beginGroup(QStringLiteral("sources"));
    const int count = m_overridesList->count();
    s.beginWriteArray(QStringLiteral("overrides"), count);
    for (int i = 0; i < count; ++i)
    {
        s.setArrayIndex(i);
        s.setValue(QStringLiteral("appId"),
                   m_overridesList->item(i)->data(Qt::UserRole));
        s.setValue(QStringLiteral("source"),
                   m_overridesList->item(i)->data(Qt::UserRole + 1));
    }
    s.endArray();
    s.endGroup();
}

