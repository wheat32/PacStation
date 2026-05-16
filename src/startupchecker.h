#pragma once

#include <QStringList>

// ============================================================
// StartupChecker – runs once at app startup to detect which
// optional system tools are installed and caches the results.
//
// Usage:
//   if (StartupChecker::instance().hasAnyAurHelper()) { … }
// ============================================================
class StartupChecker
{
public:
    static StartupChecker& instance();

    // ── AUR helpers ──────────────────────────────────────────
    /// true if yay is on the PATH.
    bool hasYay()  const { return m_hasYay;  }
    /// true if paru is on the PATH.
    bool hasParu() const { return m_hasParu; }
    /// true if at least one supported AUR helper is installed.
    bool hasAnyAurHelper() const { return m_hasYay || m_hasParu; }
    /// Returns installed AUR helpers in preference order (paru first, then yay).
    QStringList installedAurHelpers() const;

    // Add further checks here as the app grows (e.g. flatpak, reflector …)

private:
    StartupChecker();   ///< Runs all checks immediately on construction.
    void runChecks();

    bool m_hasYay  = false;
    bool m_hasParu = false;
};

