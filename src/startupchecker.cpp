#include "startupchecker.h"
#include "debug.h"

#include <QStandardPaths>

// ---------------------------------------------------------------------------
StartupChecker& StartupChecker::instance()
{
    static StartupChecker inst;
    return inst;
}

// ---------------------------------------------------------------------------
StartupChecker::StartupChecker()
{
    runChecks();
}

// ---------------------------------------------------------------------------
void StartupChecker::runChecks()
{
    DBG_CLI(QStringLiteral("--- AUR helper detection ---"));

    const QString yayPath  = QStandardPaths::findExecutable(QStringLiteral("yay"));
    const QString paruPath = QStandardPaths::findExecutable(QStringLiteral("paru"));

    m_hasYay  = !yayPath.isEmpty();
    m_hasParu = !paruPath.isEmpty();

    DBG_CLI(QStringLiteral("yay  : ") + (m_hasYay  ? yayPath  : QStringLiteral("not found")));
    DBG_CLI(QStringLiteral("paru : ") + (m_hasParu ? paruPath : QStringLiteral("not found")));

    if (!hasAnyAurHelper())
        DBG_CLI(QStringLiteral("No supported AUR helper found – AUR source will be disabled"));
}

// ---------------------------------------------------------------------------
QStringList StartupChecker::installedAurHelpers() const
{
    QStringList list;
    // Paru is generally considered the more modern/maintained helper,
    // so offer it first when both are available.
    if (m_hasParu) list << QStringLiteral("paru");
    if (m_hasYay)  list << QStringLiteral("yay");
    return list;
}
