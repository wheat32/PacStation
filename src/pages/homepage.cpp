#include "homepage.h"

#include <QLabel>
#include <QVBoxLayout>

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* label = new QLabel(QStringLiteral("This is the Home page"), this);
    label->setObjectName(QStringLiteral("pagePlaceholder"));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

