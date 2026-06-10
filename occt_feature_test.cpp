#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d_DirectionalLight.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Prs3d_Drawer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Quantity_Color.hxx>

int main()
{
    Handle(Aspect_DisplayConnection) dc = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(dc);
    Handle(V3d_Viewer) viewer = new V3d_Viewer(driver);

    // Test: directional light
    Handle(V3d_DirectionalLight) light =
        new V3d_DirectionalLight(gp_Dir(0.3, -0.4, -1.0));
    light->SetIntensity(1.0);
    viewer->AddLight(light);
    light->SetEnabled(true);


    Handle(V3d_View) view = viewer->CreateView();
    Handle(AIS_InteractiveContext) ctx = new AIS_InteractiveContext(viewer);

    // Test: AIS_Shape + edge attributes
    TopoDS_Shape box = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
    Handle(AIS_Shape) aisBox = new AIS_Shape(box);

    aisBox->SetDisplayMode(AIS_Shaded);
    aisBox->Attributes()->SetFaceBoundaryDraw(true);
    aisBox->Attributes()->FaceBoundaryAspect()->SetColor(Quantity_NOC_BLACK);



    ctx->Display(aisBox, Standard_True);

    return 0;
}
