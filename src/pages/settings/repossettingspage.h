#pragma once

#include <QWidget>

class QListWidget;

// ============================================================
// ReposSettingsPage – manage package sources/repositories.
//
// Sections:
//   • Pacman Mirrors  – nav row → MirrorsSettingsPage
//   • Pacman Repositories – editable list of [repo] sections
//   • AUR Helper      – combobox to choose yay / paru / etc.
//   • Flatpak Remotes – add/remove Flatpak remotes (Flathub
//     is added automatically on first run)
// ============================================================
class ReposSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReposSettingsPage(QWidget* parent = nullptr);

signals:
    void backRequested();
    void navigateToMirrors();

private slots:
    void onAddPacmanRepo();
    void onRemovePacmanRepo();
    void onAddFlatpakRemote();
    void onRemoveFlatpakRemote();

private:
    void buildUi();
    void loadSettings();
    void savePacmanRepos();
    void saveFlatpakRemotes();

    QListWidget* m_pacmanReposList    = nullptr;
    QListWidget* m_flatpakRemotesList = nullptr;
};

