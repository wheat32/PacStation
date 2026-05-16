#pragma once

#include <QMainWindow>
#include <QStackedWidget>

class SidebarButton;
class SettingsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QStackedWidget* m_stack        = nullptr;
    SettingsPage*   m_settingsPage = nullptr;

    SidebarButton* m_btnHome      = nullptr;
    SidebarButton* m_btnInstalled = nullptr;
    SidebarButton* m_btnUpdates   = nullptr;
    SidebarButton* m_btnDownloads = nullptr;
    SidebarButton* m_btnSettings  = nullptr;

    void buildLayout();
    void buildSidebar(QWidget* sidebar);
    void buildPages();
    void applyTheme();
};
