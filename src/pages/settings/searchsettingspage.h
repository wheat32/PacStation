#pragma once
#include <QWidget>
class ToggleWithStatus;

class SearchSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SearchSettingsPage(QWidget* parent = nullptr);
signals:
    void backRequested();
private:
    void buildUi();
    void loadSettings();
    ToggleWithStatus* m_includeAur     = nullptr;
    ToggleWithStatus* m_includeFlatpak = nullptr;
    ToggleWithStatus* m_allRemotes     = nullptr;
};

