#include "mainwindow.h"
#include "sidebarbutton.h"
#include "thememanager.h"
#include "pages/homepage.h"
#include "pages/installedpage.h"
#include "pages/updatespage.h"
#include "pages/downloadspage.h"
#include "pages/settingspage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("PacStation"));
    setMinimumSize(880, 580);
    resize(1100, 700);
    buildLayout();
    applyTheme();
}

// ---------------------------------------------------------------------------
void MainWindow::buildLayout()
{
    QWidget* root           = new QWidget(this);
    QHBoxLayout* rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    setCentralWidget(root);

    // Page area must be created before the sidebar so the signal connects
    // never dereference a null m_stack pointer.
    m_stack = new QStackedWidget;
    m_stack->setObjectName(QStringLiteral("pageStack"));
    buildPages();

    // --- Sidebar ---
    QWidget* sidebar = new QWidget;
    sidebar->setObjectName(QStringLiteral("sidebar"));
    sidebar->setFixedWidth(90);
    buildSidebar(sidebar);
    rootLayout->addWidget(sidebar);

    rootLayout->addWidget(m_stack, 1);

    // Both m_btnSettings (set by buildSidebar) and m_settingsPage (set by
    // buildPages) are now valid – safe to connect.
    connect(m_btnSettings, &SidebarButton::clicked,
            m_settingsPage, &SettingsPage::navigateToHub);
}

// ---------------------------------------------------------------------------
void MainWindow::buildSidebar(QWidget* sidebar)
{
    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(5, 12, 5, 12);
    layout->setSpacing(4);
    layout->setAlignment(Qt::AlignHCenter);

    // --- Top navigation ---
    m_btnHome = new SidebarButton(
        tr("Home"),
        QStringLiteral(":/assets/house.svg"),
        QStringLiteral(":/assets/house-fill.svg"),
        sidebar);

    m_btnInstalled = new SidebarButton(
        tr("Installed"),
        QStringLiteral(":/assets/boxes.svg"),
        QStringLiteral(":/assets/boxes.svg"), // TODO replace this with a -filled variant when one exists
        sidebar);

    m_btnUpdates = new SidebarButton(
        tr("Updates"),
        QStringLiteral(":/assets/arrow-up-circle.svg"),
        QStringLiteral(":/assets/arrow-up-circle-fill.svg"),
        sidebar);

    layout->addWidget(m_btnHome,      0, Qt::AlignHCenter);
    layout->addWidget(m_btnInstalled, 0, Qt::AlignHCenter);
    layout->addWidget(m_btnUpdates,   0, Qt::AlignHCenter);

    layout->addStretch();

    // --- Bottom navigation ---
    m_btnDownloads = new SidebarButton(
        tr("Downloads"),
        QStringLiteral(":/assets/cloud-download.svg"),
        QStringLiteral(":/assets/cloud-download-fill.svg"),
        sidebar);

    m_btnSettings = new SidebarButton(
        tr("Settings"),
        QStringLiteral(":/assets/gear.svg"),
        QStringLiteral(":/assets/gear-fill.svg"),
        sidebar);

    layout->addWidget(m_btnDownloads, 0, Qt::AlignHCenter);
    layout->addWidget(m_btnSettings,  0, Qt::AlignHCenter);

    // Wire up navigation
    connect(m_btnHome, &QToolButton::toggled, this, [this](bool on)
    {
        if (on)
            m_stack->setCurrentIndex(0);
    });
    connect(m_btnInstalled, &QToolButton::toggled, this, [this](bool on)
    {
        if (on)
            m_stack->setCurrentIndex(1);
    });
    connect(m_btnUpdates, &QToolButton::toggled, this, [this](bool on)
    {
        if (on)
            m_stack->setCurrentIndex(2);
    });
    connect(m_btnDownloads, &QToolButton::toggled, this, [this](bool on)
    {
        if (on)
            m_stack->setCurrentIndex(3);
    });
    connect(m_btnSettings, &QToolButton::toggled, this, [this](bool on)
    {
        if (on)
            m_stack->setCurrentIndex(4);
    });

    // Select Home by default
    m_btnHome->setChecked(true);
}

// ---------------------------------------------------------------------------
void MainWindow::buildPages()
{
    m_stack->addWidget(new HomePage);
    m_stack->addWidget(new InstalledPage);
    m_stack->addWidget(new UpdatesPage);
    m_stack->addWidget(new DownloadsPage);

    m_settingsPage = new SettingsPage;
    m_stack->addWidget(m_settingsPage);

    m_stack->setCurrentIndex(0);
}


// ---------------------------------------------------------------------------
void MainWindow::applyTheme()
{
    const ThemeManager& tm = ThemeManager::instance();
    const QColor fg        = tm.foregroundColor();
    const QColor accent    = tm.accentColor();

    SidebarButton* const buttons[] = {
        m_btnHome, m_btnInstalled, m_btnUpdates,
        m_btnDownloads, m_btnSettings
    };
    for (SidebarButton* btn : buttons)
        btn->applyTheme(fg, accent);
}
