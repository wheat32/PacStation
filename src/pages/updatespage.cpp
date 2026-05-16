#include "updatespage.h"

#include <QLabel>
#include <QVBoxLayout>

UpdatesPage::UpdatesPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* label = new QLabel(QStringLiteral("This is the Updates page"), this);
    label->setObjectName(QStringLiteral("pagePlaceholder"));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

