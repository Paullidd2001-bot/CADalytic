#include "Occt3DWidget.h"
#include "Occt3DView.h"

#include <QVBoxLayout>
#include <QWidget>

Occt3DWidget::Occt3DWidget(QWidget* parent)
    : QWidget(parent)
{
    m_view = new Occt3DView();

    QWidget* container = QWidget::createWindowContainer(m_view, this);
    container->setFocusPolicy(Qt::StrongFocus);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);
}
