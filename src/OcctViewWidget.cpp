#include "OcctViewWidget.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>

#include <Aspect_NeutralWindow.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <AIS_Shape.hxx>

OcctViewWidget::OcctViewWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

OcctViewWidget::~OcctViewWidget() = default;

void OcctViewWidget::initializeGL()
{
    // Display connection + graphic driver
    m_displayConnection = new Aspect_DisplayConnection();
    m_driver = new OpenGl_GraphicDriver(m_displayConnection);

    // Viewer
    m_viewer = new V3d_Viewer(m_driver);
    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();
    m_viewer->SetDefaultBackgroundColor(Quantity_NOC_WHITE);

    // View
    m_view = m_viewer->CreateView();
    m_view->SetBackgroundColor(Quantity_NOC_WHITE);
    m_view->SetProj(V3d_Zpos);

    // Context
    m_context = new AIS_InteractiveContext(m_viewer);

    // ⭐ Correct OCCT 8 window binding
    m_win = new Aspect_NeutralWindow();
    m_win->SetSize(width(), height());
    m_view->SetWindow(m_win);
    m_view->MustBeResized();

    setupScene();
}

void OcctViewWidget::setupScene()
{
    // Simple demo geometry
    TopoDS_Shape box = BRepPrimAPI_MakeBox(100.0, 80.0, 60.0).Shape();
    TopoDS_Shape cyl = BRepPrimAPI_MakeCylinder(20.0, 80.0).Shape();

    Handle(AIS_Shape) aisBox = new AIS_Shape(box);
    Handle(AIS_Shape) aisCyl = new AIS_Shape(cyl);

    aisBox->SetDisplayMode(AIS_Shaded);
    aisCyl->SetDisplayMode(AIS_Shaded);

    m_context->Display(aisBox, false);
    m_context->Display(aisCyl, false);
    m_context->UpdateCurrentViewer();

    // Trihedron
    m_view->ZBufferTriedronSetup();
    m_view->TriedronDisplay(
        Aspect_TOTP_LEFT_LOWER,
        Quantity_NOC_BLACK,
        0.08,
        V3d_ZBUFFER
    );

    // Grid
    m_viewer->SetPrivilegedPlane(gp_Ax3(gp_Pnt(0,0,0), gp_Dir(0,0,1)));
    m_viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);

    fitAll();
}

void OcctViewWidget::resizeEvent(QResizeEvent* event)
{
    QOpenGLWidget::resizeEvent(event);

    if (m_view.IsNull() || m_win.IsNull())
        return;

    const QSize s = event->size();
    m_win->SetSize(s.width(), s.height());

    m_view->Invalidate();
    m_view->MustBeResized();
}

void OcctViewWidget::resizeGL(int, int)
{
    if (!m_view.IsNull())
        m_view->Redraw();
}

void OcctViewWidget::paintGL()
{
    if (!m_view.IsNull())
        m_view->Redraw();
}

void OcctViewWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastPos = event->pos();

    if (m_view.IsNull())
        return;

    if (event->button() == Qt::LeftButton)
    {
        m_view->StartRotation(event->position().x(), event->position().y());
    }

    QOpenGLWidget::mousePressEvent(event);
}

void OcctViewWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_view.IsNull())
        return;

    const QPoint pos = event->pos();
    const QPoint delta = pos - m_lastPos;
    m_lastPos = pos;

    if (event->buttons() & Qt::LeftButton)
    {
        m_view->Rotation(event->position().x(), event->position().y());
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        m_view->Pan(delta.x(), -delta.y());
    }

    update();
    QOpenGLWidget::mouseMoveEvent(event);
}

void OcctViewWidget::wheelEvent(QWheelEvent* event)
{
    if (m_view.IsNull())
        return;

    const int delta = event->angleDelta().y();
    const double factor = (delta > 0) ? 1.1 : 0.9;

    const double cx = width() * 0.5;
    const double cy = height() * 0.5;

    int dx = int(cx + (cx * (factor - 1.0)));
    int dy = int(cy + (cy * (factor - 1.0)));

    m_view->Zoom(cx, cy, dx, dy);

    update();
    QOpenGLWidget::wheelEvent(event);
}

void OcctViewWidget::fitAll()
{
    if (m_view.IsNull())
        return;

    m_view->FitAll();
    update();
}

void OcctViewWidget::resetView()
{
    if (m_view.IsNull())
        return;

    m_view->SetProj(V3d_Zpos);
    m_view->FitAll();
    update();
}
