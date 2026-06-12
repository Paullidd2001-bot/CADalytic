#include "Occt3DView.h"

#include <QSurfaceFormat>
#include <QDebug>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <AIS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Version.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <NCollection_Vec2.hxx>

Occt3DView::Occt3DView(QWindow* parent)
    : QWindow(parent)
{
    setSurfaceType(QSurface::OpenGLSurface);

    // ⭐ DO NOT set a custom format here.
    // The global format from main.cpp MUST control the GL profile.
}

Occt3DView::~Occt3DView()
{
    // No custom GL context to delete
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

    // OCCT display + driver
    m_displayConnection = new Aspect_DisplayConnection();
    m_driver = new OpenGl_GraphicDriver(m_displayConnection);

    // Viewer + view
    m_viewer = new V3d_Viewer(m_driver);

    std::cout << "OCCT runtime version: " << OCC_VERSION_COMPLETE << std::endl;

    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();
    m_viewer->SetDefaultBackgroundColor(Quantity_NOC_WHITE);

    m_view = m_viewer->CreateView();
    m_view->SetBackgroundColor(Quantity_NOC_WHITE);
    m_view->SetProj(V3d_Zpos);

    // Trihedron
    m_view->ZBufferTriedronSetup();
    m_view->TriedronDisplay(
        Aspect_TOTP_LEFT_LOWER,
        Quantity_NOC_BLACK,
        0.08,
        V3d_ZBUFFER
    );

    // Background gradient
    m_view->SetBgGradientColors(
        Quantity_Color(0.15, 0.25, 0.55, Quantity_TOC_RGB),
        Quantity_Color(0.90, 0.90, 0.95, Quantity_TOC_RGB),
        Aspect_GFM_VER,
        true
    );

    // OCCT window wrapper
    m_occtWindow = new Aspect_NeutralWindow();
    m_occtWindow->SetNativeHandle((Aspect_Drawable)winId());
    m_occtWindow->SetPosition(0, 0);
    m_occtWindow->SetSize(width(), height());
    m_view->SetWindow(m_occtWindow);

    // AIS context
    m_context = new AIS_InteractiveContext(m_viewer);
    m_context->SetDisplayMode(AIS_Shaded, true);
    m_context->SetAutomaticHilight(true);
    m_context->Activate(0);
    // Selection Modes
    m_context->Deactivate();
    m_context->Activate(AIS_Shape::SelectionMode(TopAbs_SHAPE));   // whole shape
    m_context->Activate(AIS_Shape::SelectionMode(TopAbs_VERTEX));  // vertex
    m_context->Activate(AIS_Shape::SelectionMode(TopAbs_EDGE));    // edge
    m_context->Activate(AIS_Shape::SelectionMode(TopAbs_FACE));    // face

    // ViewCube – diagnostic version
    m_viewCube = new AIS_ViewCube();
    m_viewCube->SetSize(200.0);  // big
    m_viewCube->SetColor(Quantity_Color(1.0, 0.0, 0.0, Quantity_TOC_RGB)); // bright red
    m_viewCube->SetBoxColor(Quantity_Color(1.0, 0.8, 0.8, Quantity_TOC_RGB));
    m_viewCube->SetTextColor(Quantity_NOC_BLACK);
    m_viewCube->SetAutoHilight(true);

    // ❌ no transform persistence
    // ❌ no ZLayer
    // ❌ no offset
    // just a plain 3D object in the scene
    m_context->Display(m_viewCube, AIS_Shaded, 0, false);

    // Make sure it's in view
    m_view->FitAll();

    // Put cube on topmost 2D layer
    //m_viewCube->SetTransformPersistence(aPers);
    m_viewCube->SetZLayer(Graphic3d_ZLayerId_Topmost);

    m_context->Display(m_viewCube, AIS_Shaded, 0, false);

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

    auto sphere = new AIS_Shape(BRepPrimAPI_MakeSphere(40).Shape());
    auto cone   = new AIS_Shape(BRepPrimAPI_MakeCone(10, 5, 60).Shape());
    auto torus  = new AIS_Shape(BRepPrimAPI_MakeTorus(40, 10).Shape());

    m_context->Display(sphere, false);
    m_context->Display(cone, false);
    m_context->Display(torus, false);

    // Position the demo shapes so they don't overlap

    // Move box left
    gp_Trsf trBox;
    trBox.SetTranslation(gp_Vec(-150, 0, 0));
    box->SetLocalTransformation(trBox);

    // Move cylinder right
    gp_Trsf trCyl;
    trCyl.SetTranslation(gp_Vec(150, 0, 0));
    cyl->SetLocalTransformation(trCyl);

    // Move sphere up
    gp_Trsf trSphere;
    trSphere.SetTranslation(gp_Vec(0, 150, 0));
    sphere->SetLocalTransformation(trSphere);

    // Move torus down
    gp_Trsf trTorus;
    trTorus.SetTranslation(gp_Vec(0, -150, 0));
    torus->SetLocalTransformation(trTorus);

    // Move cone forward (towards camera)
    gp_Trsf trCone;
    trCone.SetTranslation(gp_Vec(0, 0, 150));
    cone->SetLocalTransformation(trCone);

    // Colour Objects
    box->SetColor(Quantity_NOC_RED);
    cyl->SetColor(Quantity_NOC_BLUE1);
    sphere->SetColor(Quantity_NOC_GREEN);
    torus->SetColor(Quantity_NOC_YELLOW);
    cone->SetColor(Quantity_NOC_MAGENTA1);

    m_context->UpdateCurrentViewer();

    m_initialized = true;
}

void Occt3DView::renderOcct()
{
    if (!m_initialized)
        return;

    m_view->Redraw();
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
        // Preselection
        m_context->MoveTo(event->position().x(), event->position().y(), m_view, true);

        // Selection
        if (event->modifiers() & Qt::ShiftModifier)
            m_context->SelectDetected(AIS_SelectionScheme_XOR);
        else
            m_context->SelectDetected(AIS_SelectionScheme_Replace);

        // Start rotation
        if (!m_view.IsNull())
            m_view->StartRotation(event->position().x(), event->position().y());

        renderOcct();
        return;
    }

    if (event->button() == Qt::MiddleButton)
    {
        // Middle button does not select — only pan
        return;
    }
}

void Occt3DView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    QPoint delta = pos - m_lastPos;
    m_lastPos = pos;

    if (m_view.IsNull())
        return;

    if (!(event->buttons() & Qt::LeftButton) &&
        !(event->buttons() & Qt::MiddleButton))
    {
        m_context->MoveTo(pos.x(), pos.y(), m_view, true);
        renderOcct();
        return;
    }

    if (event->buttons() & Qt::LeftButton)
    {
        m_view->Rotation(pos.x(), pos.y());
        m_view->Invalidate();
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        m_view->Pan(delta.x(), -delta.y());
        m_view->Invalidate();
    }

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
    m_view->Invalidate();
    renderOcct();
}
