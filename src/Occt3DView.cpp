#include "Occt3DView.h"

#include <QSurfaceFormat>
#include <QDebug>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <AIS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Version.hxx>

Occt3DView::Occt3DView(QWindow* parent)
    : QWindow(parent)
{
    setSurfaceType(QSurface::OpenGLSurface);

    setMouseGrabEnabled(true);
    requestActivate();

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(4, 5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(fmt);
}


Occt3DView::~Occt3DView()
{
    delete m_glContext;
}

Handle(V3d_View) Occt3DView::view() const
{
    return m_view;
}

void Occt3DView::fitAll()
{
    if (!m_view.IsNull())
    {
        m_view->FitAll();
        renderOcct();
    }
}

void Occt3DView::resetView()
{
    if (!m_view.IsNull())
    {
        m_view->SetProj(V3d_Zpos);
        m_view->FitAll();
        renderOcct();
    }
}

void Occt3DView::initOcct()
{
    if (m_initialized)
        return;

    // Create OpenGL context
    m_glContext = new QOpenGLContext(this);
    m_glContext->setFormat(format());
    if (!m_glContext->create())
    {
        qDebug() << "Failed to create QOpenGLContext";
        return;
    }

    if (!m_glContext->makeCurrent(this))
    {
        qDebug() << "Failed to make QOpenGLContext current";
        return;
    }

    // OCCT display + driver
    m_displayConnection = new Aspect_DisplayConnection();
    m_driver = new OpenGl_GraphicDriver(m_displayConnection);

    // Viewer + view
    m_viewer = new V3d_Viewer(m_driver);

    // Print OCCT runtime version
    std::cout << "OCCT runtime version: " << OCC_VERSION_COMPLETE << std::endl;

    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();
    m_viewer->SetDefaultBackgroundColor(Quantity_NOC_WHITE);

    m_view = m_viewer->CreateView();
    m_view->SetBackgroundColor(Quantity_NOC_WHITE);
    m_view->SetProj(V3d_Zpos);

    // Classic CAD gradient background
    m_view->SetBgGradientColors(
        Quantity_Color(0.15, 0.25, 0.55, Quantity_TOC_RGB),  // top
        Quantity_Color(0.90, 0.90, 0.95, Quantity_TOC_RGB),  // bottom
        Aspect_GFM_VER,
        true
    );

    // OCCT window wrapper
    m_occtWindow = new Aspect_NeutralWindow();
    m_occtWindow->SetNativeHandle((Aspect_Drawable)winId());
    m_occtWindow->SetSize(width(), height());
    m_view->SetWindow(m_occtWindow);

    // AIS context
    m_context = new AIS_InteractiveContext(m_viewer);

    // Enable OCCT 8 selection system
    m_context->SetDisplayMode(AIS_Shaded, true);
    m_context->SetAutomaticHilight(true);
    m_context->Activate(0); // default selection mode



    // ViewCube (OCCT 8.0.0 minimal API)
    m_viewCube = new AIS_ViewCube();

    // Make cube smaller so it doesn't clip off-screen
    m_viewCube->SetSize(50.0);  // was 70.0

    m_viewCube->SetBoxColor(Quantity_Color(0.95, 0.95, 0.95, Quantity_TOC_RGB));
    m_viewCube->SetTextColor(Quantity_NOC_BLACK);
    m_viewCube->SetColor(Quantity_Color(0.25, 0.25, 0.25, Quantity_TOC_RGB));
    m_viewCube->SetAutoHilight(true);

    // Only supported transform persistence constructor
    Handle(Graphic3d_TransformPers) aPers =
        new Graphic3d_TransformPers(
            Graphic3d_TMF_2d,
            Aspect_TOTP_RIGHT_UPPER
        );

    m_viewCube->SetTransformPersistence(aPers);

    // Display cube
    m_context->Display(m_viewCube, false);
    m_context->Activate(m_viewCube, 0, true);

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

    // Demo geometry
    Handle(AIS_Shape) box =
        new AIS_Shape(BRepPrimAPI_MakeBox(100, 80, 60).Shape());
    Handle(AIS_Shape) cyl =
        new AIS_Shape(BRepPrimAPI_MakeCylinder(20, 80).Shape());

    box->SetDisplayMode(AIS_Shaded);
    cyl->SetDisplayMode(AIS_Shaded);

    m_context->Display(box, false);
    m_context->Display(cyl, false);

    m_context->Activate(box, 0);
    m_context->Activate(cyl, 0);

    m_context->UpdateCurrentViewer();

    m_initialized = true;
}

void Occt3DView::renderOcct()
{
    if (!m_initialized)
        return;

    if (!m_glContext->makeCurrent(this))
        return;

    m_view->Redraw();
    m_glContext->swapBuffers(this);
}

void Occt3DView::exposeEvent(QExposeEvent* event)
{
    Q_UNUSED(event);

    if (isExposed())
    {
        if (!m_initialized)
            initOcct();

        renderOcct();
    }
}

void Occt3DView::resizeEvent(QResizeEvent* event)
{
    QWindow::resizeEvent(event);

    if (!m_initialized)
        return;

    m_occtWindow->SetSize(width(), height());
    m_view->MustBeResized();
    renderOcct();
}

void Occt3DView::mousePressEvent(QMouseEvent* event)
{
    m_lastPos = event->pos();

    if (event->button() == Qt::LeftButton)
    {
        // Detect what is under the cursor
        m_context->MoveTo(event->position().x(), event->position().y(), m_view, true);

        if (event->modifiers() & Qt::ShiftModifier)
            m_context->SelectDetected(AIS_SelectionScheme_XOR);
        else
            m_context->SelectDetected(AIS_SelectionScheme_Replace);

        renderOcct();
        return;
    }

    // Rotation start
    if (event->button() == Qt::LeftButton && !m_view.IsNull())
        m_view->StartRotation(event->position().x(), event->position().y());
}

void Occt3DView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    QPoint delta = pos - m_lastPos;
    m_lastPos = pos;

    if (m_view.IsNull())
        return;

    // Hover highlight
    if (!(event->buttons() & Qt::LeftButton) &&
        !(event->buttons() & Qt::MiddleButton))
    {
        m_context->MoveTo(pos.x(), pos.y(), m_view, true);
        renderOcct();
        return;
    }

    // Rotate / pan
    if (event->buttons() & Qt::LeftButton)
        m_view->Rotation(pos.x(), pos.y());
    else if (event->buttons() & Qt::MiddleButton)
        m_view->Pan(delta.x(), -delta.y());

    renderOcct();
}

void Occt3DView::wheelEvent(QWheelEvent* event)
{
    if (m_view.IsNull())
        return;

    double factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;

    int cx = width() / 2;
    int cy = height() / 2;

    int dx = int(cx + (cx * (factor - 1.0)));
    int dy = int(cy + (cy * (factor - 1.0)));

    m_view->Zoom(cx, cy, dx, dy);
    renderOcct();
}
