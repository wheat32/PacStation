#pragma once
#include <QWidget>
class QComboBox;

class AppearanceSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit AppearanceSettingsPage(QWidget* parent = nullptr);
signals:
    void backRequested();
private:
    void buildUi();
    void loadSettings();
    QComboBox* m_themeCombo   = nullptr;
    QComboBox* m_densityCombo = nullptr;
};

