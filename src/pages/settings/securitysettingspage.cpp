#include "securitysettingspage.h"
#include "widgets/settingrow.h"
#include "widgets/togglewithstatus.h"
#include "widgets/toastnotification.h"
#include "debug.h"

#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
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
SecuritySettingsPage::SecuritySettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadSettings();
    refreshCacheSizes();
}

// ---------------------------------------------------------------------------
void SecuritySettingsPage::buildUi()
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
            this, &SecuritySettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Security & Trust"), topBar);
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

    // ── Section: AUR Warnings ────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("AUR Safety"), content));

    m_warnAur = new ToggleWithStatus(nullptr);
    auto* warnRow = new SettingRow(
        tr("Warn Before Installing AUR Packages"),
        tr("Show a warning dialog before installing any AUR package, reminding you "
           "that AUR packages are community-maintained and not officially vetted."),
        m_warnAur, content);
    layout->addWidget(warnRow);

    layout->addSpacing(16);

    // ── Section: Cache ───────────────────────────────────────
    layout->addWidget(makeSectionHeader(tr("Cache Management"), content));

    // Pacman cache size row
    m_pacmanCacheSize = new QLabel(tr("Calculating…"), nullptr);
    m_pacmanCacheSize->setObjectName(QStringLiteral("settingRowSubtitle"));
    auto* pacCacheRow = new SettingRow(
        tr("Pacman Package Cache"),
        tr("Cached package archives in /var/cache/pacman/pkg."),
        m_pacmanCacheSize, content);
    layout->addWidget(pacCacheRow);

    // Flatpak cache size row
    m_flatpakCacheSize = new QLabel(tr("Calculating…"), nullptr);
    m_flatpakCacheSize->setObjectName(QStringLiteral("settingRowSubtitle"));
    auto* fpCacheRow = new SettingRow(
        tr("Flatpak Unused Runtimes"),
        tr("Flatpak runtimes and extensions no longer used by any installed app."),
        m_flatpakCacheSize, content);
    layout->addWidget(fpCacheRow);

    // Clean button
    auto* cleanBtn = new QPushButton(tr("Clean All Caches"), content);
    cleanBtn->setObjectName(QStringLiteral("settingsSaveBtn"));
    cleanBtn->setCursor(Qt::PointingHandCursor);
    cleanBtn->setFixedWidth(200);
    connect(cleanBtn, &QPushButton::clicked,
            this, &SecuritySettingsPage::onCleanCache);
    layout->addWidget(cleanBtn);

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void SecuritySettingsPage::loadSettings()
{
    QSettings s;
    const bool warn = s.value(QStringLiteral("security/warnAur"), true).toBool();
    DBG_SETTINGS(QStringLiteral("security/warnAur = ") + (warn ? QStringLiteral("true") : QStringLiteral("false")));
    m_warnAur->setOn(warn, false);

    // Wire future saves
    connect(m_warnAur, &ToggleWithStatus::toggled, this, [](bool on)
    {
        DBG_SETTINGS(QStringLiteral("security/warnAur -> ") + (on ? QStringLiteral("true") : QStringLiteral("false")));
        QSettings().setValue(QStringLiteral("security/warnAur"), on);
    });
}

// ---------------------------------------------------------------------------
void SecuritySettingsPage::refreshCacheSizes()
{
    DBG_CLI(QStringLiteral("Querying pacman cache size: du -sh /var/cache/pacman/pkg"));

    auto* pacProc = new QProcess(this);
    connect(pacProc, &QProcess::finished, this,
            [this, pacProc](int exitCode, QProcess::ExitStatus)
    {
        if (exitCode == 0)
        {
            const QString result = QString::fromUtf8(pacProc->readAllStandardOutput())
                                       .trimmed()
                                       .section(QLatin1Char('\t'), 0, 0);
            DBG_CLI(QStringLiteral("Pacman cache size: ") + result);
            m_pacmanCacheSize->setText(result);
        }
        else
        {
            DBG_CLI(QStringLiteral("du failed – exit code %1").arg(exitCode));
            m_pacmanCacheSize->setText(tr("Unknown"));
        }
        pacProc->deleteLater();
    });
    pacProc->start(QStringLiteral("du"),
                   { QStringLiteral("-sh"),
                     QStringLiteral("/var/cache/pacman/pkg") });

    m_flatpakCacheSize->setText(tr("Run 'flatpak uninstall --unused' to check"));
}

// ---------------------------------------------------------------------------
void SecuritySettingsPage::onCleanCache()
{
    DBG_CLI(QStringLiteral("Launching: pkexec paccache -r"));

    auto* proc = new QProcess(this);
    proc->setProgram(QStringLiteral("pkexec"));
    proc->setArguments({ QStringLiteral("paccache"), QStringLiteral("-r") });
    connect(proc, &QProcess::finished, this,
            [this, proc](int exitCode, QProcess::ExitStatus)
    {
        if (exitCode == 0)
        {
            DBG_CLI(QStringLiteral("paccache -r succeeded"));
            ToastNotification::popup(this, tr("Cache cleaned successfully."));
        }
        else
        {
            DBG_CLI(QStringLiteral("paccache -r failed – exit code %1").arg(exitCode));
            ToastNotification::popup(this,
                tr("Cache clean failed or was cancelled (exit %1).").arg(exitCode));
        }
        refreshCacheSizes();
        proc->deleteLater();
    });
    proc->start();
}

