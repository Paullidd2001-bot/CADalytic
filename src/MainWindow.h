#pragma once

#include <QMainWindow>

class QAction;
class Occt3DWidget;   // ⭐ NEW — replaces OcctViewWidget

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

private:
    Occt3DWidget* m_view;   // ⭐ NEW — replaces OcctViewWidget*

    QAction* m_actionOpen;
    QAction* m_actionFitAll;
    QAction* m_actionResetView;
    QAction* m_actionExit;
};
