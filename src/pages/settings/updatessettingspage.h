#pragma once
#include <QWidget>
class ToggleWithStatus;
class QComboBox;

class UpdatesSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit UpdatesSettingsPage(QWidget* parent = nullptr);
signals:
    void backRequested();
private slots:
    void onNotificationsToggled(bool on);
private:
    void buildUi();
    void loadSettings();
    ToggleWithStatus* m_notifyToggle      = nullptr;
    QWidget*          m_notifyOptions     = nullptr;
    QComboBox*        m_frequencyCombo    = nullptr;
    ToggleWithStatus* m_notifyPacman      = nullptr;
    ToggleWithStatus* m_notifyAur         = nullptr;
    ToggleWithStatus* m_notifyFlatpak     = nullptr;
};

