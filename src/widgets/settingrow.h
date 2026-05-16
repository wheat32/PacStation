#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>

// ============================================================
// SettingRow – a single Microsoft-Store-style settings row.
//
// Layout:  [icon placeholder]  [title / subtitle]  [control]
//
// • The icon placeholder (36×36) is reserved for a future
//   icon pass; leave it empty for now.
// • Pass any QWidget* as the control (ToggleSwitch, QComboBox,
//   QPushButton, …).
// • Call makeNavigationRow() to instead show a "›" chevron and
//   emit clicked() when the row is pressed.
// ============================================================
class SettingRow : public QFrame
{
    Q_OBJECT

public:
    explicit SettingRow(const QString& title,
                        const QString& subtitle = {},
                        QWidget*       control  = nullptr,
                        QWidget*       parent   = nullptr);

    /// Replace (or set) the control widget on the right side.
    void setControl(QWidget* control);

    /// Turn the row into a navigation row: shows a "›" chevron
    /// and emits clicked() on press.
    void makeNavigationRow();

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    QHBoxLayout* m_mainLayout    = nullptr;
    QLabel*      m_titleLabel    = nullptr;
    QLabel*      m_subtitleLabel = nullptr;
    QWidget*     m_control       = nullptr;
    bool         m_isNavRow      = false;
};

