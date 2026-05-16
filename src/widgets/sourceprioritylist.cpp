#include "sourceprioritylist.h"
#include "toggleswitch.h"

#include <QDropEvent>
#include <QHBoxLayout>
#include <QLabel>

// Roles stored on every QListWidgetItem
static constexpr int RoleId      = Qt::UserRole;
static constexpr int RoleName    = Qt::UserRole + 1;
static constexpr int RoleEnabled = Qt::UserRole + 2;

// ---------------------------------------------------------------------------
SourcePriorityList::SourcePriorityList(QWidget* parent)
    : QListWidget(parent)
{
    setObjectName(QStringLiteral("sourcePriorityList"));
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSpacing(3);
    setFrameShape(QFrame::NoFrame);
}

// ---------------------------------------------------------------------------
void SourcePriorityList::setSources(const QList<SourceEntry>& sources)
{
    clear();
    for (const SourceEntry& src : sources)
    {
        auto* item = new QListWidgetItem(this);
        item->setData(RoleId,      src.id);
        item->setData(RoleName,    src.name);
        item->setData(RoleEnabled, src.enabled);
        item->setSizeHint(QSize(0, 58));
        addItem(item);
    }
    rebuildItemWidgets();
}

// ---------------------------------------------------------------------------
QList<SourceEntry> SourcePriorityList::sources() const
{
    QList<SourceEntry> result;
    for (int i = 0; i < count(); ++i)
    {
        const QListWidgetItem* it = item(i);
        result.append({
            .id      = it->data(RoleId).toString(),
            .name    = it->data(RoleName).toString(),
            .enabled = it->data(RoleEnabled).toBool()
        });
    }
    return result;
}

// ---------------------------------------------------------------------------
void SourcePriorityList::dropEvent(QDropEvent* e)
{
    QListWidget::dropEvent(e);
    rebuildItemWidgets();
    emit sourcesChanged();
}

// ---------------------------------------------------------------------------
void SourcePriorityList::setSourceDisabled(const QString& id, bool disabled)
{
    if (disabled)
        m_disabledIds.insert(id);
    else
        m_disabledIds.remove(id);
    rebuildItemWidgets();
}

// ---------------------------------------------------------------------------
void SourcePriorityList::rebuildItemWidgets()
{
    for (int i = 0; i < count(); ++i)
    {
        QListWidgetItem* it = item(i);
        const bool    enabled    = it->data(RoleEnabled).toBool();
        const QString name       = it->data(RoleName).toString();
        const QString id         = it->data(RoleId).toString();
        const bool    srcDisabled = m_disabledIds.contains(id);

        auto* row    = new QWidget(this);
        auto* layout = new QHBoxLayout(row);
        layout->setContentsMargins(12, 8, 12, 8);
        layout->setSpacing(12);

        // Drag handle
        auto* handle = new QLabel(QStringLiteral("⠿"), row);
        handle->setObjectName(QStringLiteral("dragHandle"));
        handle->setCursor(Qt::SizeAllCursor);
        layout->addWidget(handle);

        // Source name (+ "not installed" note for disabled sources)
        auto* nameLabel = new QLabel(
            srcDisabled
                ? QStringLiteral("%1  <span style='font-size:11px;color:gray;'>(%2)</span>")
                      .arg(name, tr("not installed"))
                : name,
            row);
        nameLabel->setTextFormat(Qt::RichText);
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(nameLabel, 1);

        // Enable/disable toggle – greyed out and non-interactive when source is unavailable
        auto* toggle = new ToggleSwitch(row);
        toggle->setOn(srcDisabled ? false : enabled, /*animate=*/false);
        toggle->setEnabled(!srcDisabled);

        const int capturedRow = i;
        connect(toggle, &ToggleSwitch::toggled, this, [this, capturedRow](bool on)
        {
            if (QListWidgetItem* target = this->item(capturedRow))
                target->setData(RoleEnabled, on);
            emit sourcesChanged();
        });

        layout->addWidget(toggle);
        setItemWidget(it, row);
    }
}

