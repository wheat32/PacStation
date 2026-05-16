#include "downloadspage.h"

#include <QLabel>
#include <QVBoxLayout>

DownloadsPage::DownloadsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* label = new QLabel(QStringLiteral("This is the Downloads page"), this);
    label->setObjectName(QStringLiteral("pagePlaceholder"));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

