#pragma once

#include <QWidget>

class QListWidget;
class QPushButton;
class QLabel;
class QProcess;

// ============================================================
// MirrorsSettingsPage – editable Pacman mirror list.
//
// The mirrorlist file (/etc/pacman.d/mirrorlist) is world-
// readable, so it is loaded directly.  Saving requires root;
// the Save button runs the write via pkexec.
//
// A "Refresh with Reflector" button auto-ranks mirrors by
// speed/latency using the reflector tool (if installed).
// ============================================================
class MirrorsSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit MirrorsSettingsPage(QWidget* parent = nullptr);

signals:
    void backRequested();

private slots:
    void onSave();
    void onRefreshWithReflector();
    void onAddMirror();
    void onRemoveMirror();

private:
    void buildUi();
    void loadMirrors();

    QListWidget* m_mirrorList       = nullptr;
    QPushButton* m_saveBtn          = nullptr;
    QPushButton* m_reflectorBtn     = nullptr;
    QLabel*      m_statusLabel      = nullptr;
    QProcess*    m_process          = nullptr;

    static constexpr const char* kMirrorlistPath = "/etc/pacman.d/mirrorlist";
};

