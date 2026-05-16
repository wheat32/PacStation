#pragma once

#include <QListWidget>
#include <QSet>

// ============================================================
// SourceEntry – lightweight data record for one installable
// source (e.g. "pacman", "aur", "flathub", custom remote).
// ============================================================
struct SourceEntry
{
    QString id;       ///< Stable identifier stored in QSettings
    QString name;     ///< Localised display name
    bool    enabled = true;
};

// ============================================================
// SourcePriorityList – drag-to-reorder QListWidget where every
// row shows a drag handle, the source name, and an enable
// toggle.  After any reorder or toggle change sourcesChanged()
// is emitted.
//
// Usage:
//   auto* list = new SourcePriorityList(this);
//   list->setSources({ {"pacman", tr("Official Repos"), true},
//                      {"aur",    tr("AUR"),            true},
//                      {"flathub",tr("Flathub"),        true} });
//   connect(list, &SourcePriorityList::sourcesChanged, …);
// ============================================================
class SourcePriorityList : public QListWidget
{
    Q_OBJECT

public:
    explicit SourcePriorityList(QWidget* parent = nullptr);

    void setSources(const QList<SourceEntry>& sources);
    [[nodiscard]] QList<SourceEntry> sources() const;

    /// Mark a source as permanently disabled (e.g. AUR when no AUR helper is
    /// installed). The row stays visible so the user knows why it is
    /// unavailable, but the toggle is greyed out and non-interactive.
    void setSourceDisabled(const QString& id, bool disabled);

signals:
    void sourcesChanged();

protected:
    void dropEvent(QDropEvent* e) override;

private:
    void rebuildItemWidgets();

    QSet<QString> m_disabledIds;
};
