#pragma once

#include <QWidget>

class SourcePriorityList;
class ToggleWithStatus;
class QListWidget;

// ============================================================
// SourcesSettingsPage – "Sources" settings sub-page.
//
// Sections:
//   1. Source Priority  – drag-to-reorder list with per-source
//      enable/disable toggles.
//   2. Developer Recommendations – master toggle that enables
//      per-app source overrides, plus the override list itself.
// ============================================================
class SourcesSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SourcesSettingsPage(QWidget* parent = nullptr);

signals:
    void backRequested();

private slots:
    void onHonorToggled(bool on);
    void onAddOverride();
    void onRemoveOverride();

private:
    void buildUi();
    void loadSettings();
    void saveSourceOrder();

    SourcePriorityList* m_priorityList      = nullptr;
    ToggleWithStatus*   m_honorToggle       = nullptr;
    QWidget*            m_overridesSection  = nullptr;
    QListWidget*        m_overridesList     = nullptr;
};

