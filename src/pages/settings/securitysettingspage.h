#pragma once
#include <QWidget>
class ToggleWithStatus;
class QLabel;

class SecuritySettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SecuritySettingsPage(QWidget* parent = nullptr);
signals:
    void backRequested();
private slots:
    void onCleanCache();
private:
    void buildUi();
    void loadSettings();
    void refreshCacheSizes();
    ToggleWithStatus* m_warnAur          = nullptr;
    QLabel*           m_pacmanCacheSize  = nullptr;
    QLabel*           m_flatpakCacheSize = nullptr;
};

