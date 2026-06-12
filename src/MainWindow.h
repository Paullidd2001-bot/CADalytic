#pragma once

#include <QMainWindow>
#include "Occt3DView.h"   // ⭐ Add this include

class QAction;

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
    Occt3DView* m_view = nullptr;   // ⭐ Add this member

    QAction* m_actionOpen = nullptr;
    QAction* m_actionFitAll = nullptr;
    QAction* m_actionResetView = nullptr;
    QAction* m_actionExit = nullptr;
};
