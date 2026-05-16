#pragma once

#include <QStack>
#include <QWidget>

class QStackedWidget;
class SourcesSettingsPage;
class ReposSettingsPage;
class MirrorsSettingsPage;
class UpdatesSettingsPage;
class SearchSettingsPage;
class AppearanceSettingsPage;
class SecuritySettingsPage;

// ============================================================
// SettingsPage – top-level Settings container.
//
// Hosts a QStackedWidget whose index 0 is the settings hub
// (category list). Sub-pages are lazily created and pushed
// onto a navigation stack via navigateTo() / navigateBack().
// ============================================================
class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget* parent = nullptr);

    /// Reset the internal navigation stack back to the hub (category list).
    /// Call this when the sidebar Settings button is pressed while the user
    /// is already on a settings sub-page.
    void navigateToHub();

private:
    void buildHub(QWidget* hub);
    void navigateTo(QWidget* page);
    void navigateBack();

    QStackedWidget* m_stack = nullptr;
    QStack<QWidget*> m_history;

    // Sub-pages (created lazily on first navigation)
    SourcesSettingsPage*    m_sourcesPage    = nullptr;
    ReposSettingsPage*      m_reposPage      = nullptr;
    MirrorsSettingsPage*    m_mirrorsPage    = nullptr;
    UpdatesSettingsPage*    m_updatesPage    = nullptr;
    SearchSettingsPage*     m_searchPage     = nullptr;
    AppearanceSettingsPage* m_appearancePage = nullptr;
    SecuritySettingsPage*   m_securityPage   = nullptr;
};
