#pragma once

#include <QWidget>

class Occt3DView;

class Occt3DWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Occt3DWidget(QWidget* parent = nullptr);
    Occt3DView* view() const { return m_view; }

private:
    Occt3DView* m_view;
};
