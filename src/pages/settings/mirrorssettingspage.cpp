#include "mirrorssettingspage.h"
#include "widgets/infobanner.h"
#include "debug.h"

#include <QDateTime>
#include <QFile>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QTextStream>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
MirrorsSettingsPage::MirrorsSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    loadMirrors();
}

// ---------------------------------------------------------------------------
void MirrorsSettingsPage::buildUi()
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
    connect(backBtn, &QPushButton::clicked, this, &MirrorsSettingsPage::backRequested);
    topLayout->addWidget(backBtn);

    auto* pageTitle = new QLabel(tr("Pacman Mirrors"), topBar);
    pageTitle->setObjectName(QStringLiteral("settingsPageTitle"));
    topLayout->addWidget(pageTitle);
    topLayout->addStretch();
    outerLayout->addWidget(topBar);

    // ── Content ───────────────────────────────────────────────
    auto* scroll  = new QScrollArea(this);
    auto* content = new QWidget(scroll);
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(24, 16, 24, 32);
    layout->setSpacing(12);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll, 1);

    // Auth notice
    auto* banner = new InfoBanner(
        tr("<b>Administrator privileges required to save.</b>  Changes are written "
           "to <code>/etc/pacman.d/mirrorlist</code> via <code>pkexec</code>.  "
           "You will be prompted for your password when you click <i>Save</i>."),
        content);
    layout->addWidget(banner);

    // Description
    auto* desc = new QLabel(
        tr("Active mirrors are listed below (commented-out lines are hidden).  "
           "Drag rows to reorder.  Mirrors are tried in top-to-bottom order."),
        content);
    desc->setObjectName(QStringLiteral("settingRowSubtitle"));
    desc->setWordWrap(true);
    layout->addWidget(desc);

    // Mirror list
    m_mirrorList = new QListWidget(content);
    m_mirrorList->setObjectName(QStringLiteral("mirrorList"));
    m_mirrorList->setDragEnabled(true);
    m_mirrorList->setAcceptDrops(true);
    m_mirrorList->setDropIndicatorShown(true);
    m_mirrorList->setDragDropMode(QAbstractItemView::InternalMove);
    m_mirrorList->setMinimumHeight(200);
    m_mirrorList->setFrameShape(QFrame::NoFrame);
    layout->addWidget(m_mirrorList, 1);

    // Status label (shown after save / error)
    m_statusLabel = new QLabel(content);
    m_statusLabel->setObjectName(QStringLiteral("settingRowSubtitle"));
    m_statusLabel->setVisible(false);
    layout->addWidget(m_statusLabel);

    // ── Button row ───────────────────────────────────────────
    auto* btnRow    = new QWidget(content);
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(8);

    auto* addBtn = new QPushButton(tr("+ Add Mirror"), btnRow);
    addBtn->setObjectName(QStringLiteral("settingsAddBtn"));
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &MirrorsSettingsPage::onAddMirror);
    btnLayout->addWidget(addBtn);

    auto* removeBtn = new QPushButton(tr("Remove Selected"), btnRow);
    removeBtn->setObjectName(QStringLiteral("settingsRemoveBtn"));
    removeBtn->setCursor(Qt::PointingHandCursor);
    connect(removeBtn, &QPushButton::clicked,
            this, &MirrorsSettingsPage::onRemoveMirror);
    btnLayout->addWidget(removeBtn);

    btnLayout->addStretch();

    m_reflectorBtn = new QPushButton(tr("⟳  Refresh with Reflector"), btnRow);
    m_reflectorBtn->setObjectName(QStringLiteral("settingsAddBtn"));
    m_reflectorBtn->setCursor(Qt::PointingHandCursor);
    m_reflectorBtn->setToolTip(
        tr("Auto-rank mirrors by speed using the reflector tool.\n"
           "Requires reflector to be installed (pacman -S reflector)."));
    connect(m_reflectorBtn, &QPushButton::clicked,
            this, &MirrorsSettingsPage::onRefreshWithReflector);
    btnLayout->addWidget(m_reflectorBtn);

    m_saveBtn = new QPushButton(tr("Save"), btnRow);
    m_saveBtn->setObjectName(QStringLiteral("settingsSaveBtn"));
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveBtn, &QPushButton::clicked, this, &MirrorsSettingsPage::onSave);
    btnLayout->addWidget(m_saveBtn);

    layout->addWidget(btnRow);

    layout->addStretch();
}

// ---------------------------------------------------------------------------
void MirrorsSettingsPage::loadMirrors()
{
    DBG_CLI(QStringLiteral("Reading mirror list from ") +
            QString::fromLatin1(kMirrorlistPath));

    QFile file(QString::fromLatin1(kMirrorlistPath));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DBG_CLI(QStringLiteral("Failed to open mirrorlist for reading"));
        m_statusLabel->setText(
            tr("Could not read %1").arg(QString::fromLatin1(kMirrorlistPath)));
        m_statusLabel->setVisible(true);
        return;
    }

    QTextStream in(&file);
    int count = 0;
    while (!in.atEnd())
    {
        const QString line = in.readLine().trimmed();
        if (line.startsWith(QStringLiteral("Server"), Qt::CaseInsensitive))
        {
            const QString url = line.mid(line.indexOf(QLatin1Char('=')) + 1).trimmed();
            auto* item = new QListWidgetItem(url, m_mirrorList);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ++count;
        }
    }
    DBG_CLI(QStringLiteral("Loaded %1 active mirror(s)").arg(count));
}

// ---------------------------------------------------------------------------
void MirrorsSettingsPage::onSave()
{
    const int mirrorCount = m_mirrorList->count();
    DBG_CLI(QStringLiteral("Saving mirrorlist via pkexec tee – %1 mirror(s)")
                .arg(mirrorCount));

    QString content = QStringLiteral(
        "# Pacman mirrorlist – managed by PacStation\n"
        "# Generated: %1\n\n")
        .arg(QDateTime::currentDateTime().toString(Qt::ISODate));

    for (int i = 0; i < mirrorCount; ++i)
        content += QStringLiteral("Server = %1\n").arg(m_mirrorList->item(i)->text());

    m_process = new QProcess(this);
    m_process->setProgram(QStringLiteral("pkexec"));
    m_process->setArguments({
        QStringLiteral("tee"),
        QString::fromLatin1(kMirrorlistPath)
    });

    connect(m_process, &QProcess::finished, this,
            [this](int exitCode, QProcess::ExitStatus)
    {
        if (exitCode == 0)
        {
            DBG_CLI(QStringLiteral("pkexec tee succeeded – mirrorlist written"));
            m_statusLabel->setText(tr("✓  Mirror list saved successfully."));
        }
        else
        {
            DBG_CLI(QStringLiteral("pkexec tee failed – exit code %1").arg(exitCode));
            m_statusLabel->setText(
                tr("✗  Failed to save (exit code %1). "
                   "Authentication may have been cancelled.").arg(exitCode));
        }
        m_statusLabel->setVisible(true);
        m_saveBtn->setEnabled(true);
        m_process->deleteLater();
        m_process = nullptr;
    });

    m_saveBtn->setEnabled(false);
    m_statusLabel->setVisible(false);
    m_process->start();
    m_process->write(content.toUtf8());
    m_process->closeWriteChannel();
}

// ---------------------------------------------------------------------------
void MirrorsSettingsPage::onRefreshWithReflector()
{
    DBG_CLI(QStringLiteral("Launching: pkexec reflector --latest 20 --sort rate --save ")
            + QString::fromLatin1(kMirrorlistPath));

    m_reflectorBtn->setEnabled(false);
    m_statusLabel->setText(tr("Running reflector… this may take a moment."));
    m_statusLabel->setVisible(true);

    m_process = new QProcess(this);
    m_process->setProgram(QStringLiteral("pkexec"));
    m_process->setArguments({
        QStringLiteral("reflector"),
        QStringLiteral("--latest"), QStringLiteral("20"),
        QStringLiteral("--sort"),   QStringLiteral("rate"),
        QStringLiteral("--save"),   QString::fromLatin1(kMirrorlistPath)
    });

    connect(m_process, &QProcess::finished, this,
            [this](int exitCode, QProcess::ExitStatus)
    {
        m_reflectorBtn->setEnabled(true);
        if (exitCode == 0)
        {
            DBG_CLI(QStringLiteral("reflector succeeded – reloading mirror list"));
            m_mirrorList->clear();
            loadMirrors();
            m_statusLabel->setText(tr("✓  Mirrors refreshed with Reflector."));
        }
        else
        {
            DBG_CLI(QStringLiteral("reflector failed – exit code %1").arg(exitCode));
            m_statusLabel->setText(
                tr("✗  Reflector failed (exit code %1). "
                   "Is reflector installed?").arg(exitCode));
        }
        m_process->deleteLater();
        m_process = nullptr;
    });

    m_process->start();
}

// ---------------------------------------------------------------------------
void MirrorsSettingsPage::onAddMirror()
{
    bool ok = false;
    const QString url = QInputDialog::getText(
        this,
        tr("Add Mirror"),
        tr("Mirror URL (e.g. https://mirror.example.com/archlinux/$repo/os/$arch):"),
        QLineEdit::Normal, {}, &ok);
    if (ok && !url.trimmed().isEmpty())
    {
        DBG_CLI(QStringLiteral("Mirror added manually: ") + url.trimmed());
        auto* item = new QListWidgetItem(url.trimmed(), m_mirrorList);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
}

// ---------------------------------------------------------------------------
void MirrorsSettingsPage::onRemoveMirror()
{
    const auto selected = m_mirrorList->selectedItems();
    for (auto* item : selected)
    {
        DBG_CLI(QStringLiteral("Mirror removed: ") + item->text());
        delete item;
    }
}
