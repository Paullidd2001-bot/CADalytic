#pragma once

#include <QWindow>
#include <QOpenGLContext>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewCube.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <OpenGl_GraphicDriver.hxx>

class Occt3DView : public QWindow
{
public:
    explicit Occt3DView(QWindow* parent = nullptr);
    ~Occt3DView() override;

    Handle(V3d_View) view() const;
    void fitAll();
    void resetView();

protected:
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void initOcct();
    void renderOcct();

private:
    bool m_initialized = false;

    QOpenGLContext* m_glContext = nullptr;

    Handle(Aspect_DisplayConnection) m_displayConnection;
    Handle(OpenGl_GraphicDriver)     m_driver;
    Handle(V3d_Viewer)               m_viewer;
    Handle(V3d_View)                 m_view;
    Handle(Aspect_NeutralWindow)     m_occtWindow;
    Handle(AIS_InteractiveContext)   m_context;

    Handle(AIS_ViewCube)             m_viewCube;

    QPoint m_lastPos;
};
