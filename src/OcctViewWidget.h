#pragma once

#include <QOpenGLWidget>
#include <QPoint>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>

class OcctViewWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit OcctViewWidget(QWidget* parent = nullptr);
    ~OcctViewWidget() override;

    // These must be public so MainWindow can call them
    void fitAll();
    void resetView();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void setupScene();

private:
    Handle(Aspect_DisplayConnection) m_displayConnection;
    Handle(OpenGl_GraphicDriver)     m_driver;
    Handle(V3d_Viewer)               m_viewer;
    Handle(V3d_View)                 m_view;
    Handle(AIS_InteractiveContext)   m_context;
    Handle(Aspect_NeutralWindow) m_win;

    QPoint m_lastPos;
};
