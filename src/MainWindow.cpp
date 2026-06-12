#include "MainWindow.h"
#include "Occt3DView.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1024, 768);
    move(100, 100);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    setWindowTitle(tr("CADalytic"));

    //
    // ⭐ Create the OCCT QWindow
    //
    m_view = new Occt3DView();

    //
    // ⭐ Wrap it in a QWidget so it can be used as central widget
    //
    QWidget* container = QWidget::createWindowContainer(m_view, this);
    container->setFocusPolicy(Qt::StrongFocus);

    //
    // ⭐ Set as central widget
    //
    setCentralWidget(container);
}

MainWindow::~MainWindow() = default;

void MainWindow::createActions()
{
    // File → Open
    m_actionOpen = new QAction(tr("&Open…"), this);
    connect(m_actionOpen, &QAction::triggered, this, [this]() {
        const QString file = QFileDialog::getOpenFileName(
            this,
            tr("Open CAD File"),
            QString(),
            tr("STEP (*.stp *.step);;IGES (*.igs *.iges);;BREP (*.brep);;All Files (*.*)")
        );

        if (!file.isEmpty()) {
            statusBar()->showMessage("Loaded: " + file, 3000);
        }
    });

    // View → Fit All
    m_actionFitAll = new QAction(tr("Fit &All"), this);
    connect(m_actionFitAll, &QAction::triggered, this, [this]() {
        m_view->fitAll();
    });

    // View → Reset View
    m_actionResetView = new QAction(tr("&Reset View"), this);
    connect(m_actionResetView, &QAction::triggered, this, [this]() {
        m_view->resetView();
    });

    // File → Exit
    m_actionExit = new QAction(tr("E&xit"), this);
    connect(m_actionExit, &QAction::triggered, this, [this]() {
        close();
    });
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_actionOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionExit);

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_actionFitAll);
    viewMenu->addAction(m_actionResetView);
}

void MainWindow::createToolBars()
{
    QToolBar* fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(m_actionOpen);
    fileToolBar->addAction(m_actionExit);

    QToolBar* viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(m_actionFitAll);
    viewToolBar->addAction(m_actionResetView);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
