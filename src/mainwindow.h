#pragma once

#include <QMainWindow>
#include <QStackedWidget>

class SidebarButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QStackedWidget* m_stack = nullptr;

    SidebarButton* m_btnHome      = nullptr;
    SidebarButton* m_btnInstalled = nullptr;
    SidebarButton* m_btnUpdates   = nullptr;
    SidebarButton* m_btnDownloads = nullptr;
    SidebarButton* m_btnSettings  = nullptr;

    void buildLayout();
    void buildSidebar(QWidget* sidebar);
    void buildPages() const;
    static QWidget* makePage(const QString& title);
    void applyTheme();
};

