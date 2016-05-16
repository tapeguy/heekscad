
project( heekscad )
cmake_minimum_required( VERSION 2.6 )

if( NOT DEFINED BUILD_STARTED_IN_RIGHT_DIR )
  message( FATAL_ERROR "Don't start a build in the src directory. Do a 'cd ..', then run cmake ." )
endif( NOT DEFINED BUILD_STARTED_IN_RIGHT_DIR )

set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall -Wno-overloaded-virtual -DUSE_GENERIC_TREECTRL -DHEEKSCAD -DUNICODE -DTIXML_USE_STL )

#find OCE or OpenCASCADE
set( CASCADE_LIBS TKVRML TKSTL TKBRep TKIGES TKShHealing TKSTEP TKSTEP209 TKSTEPAttr TKSTEPBase TKXSBase TKShapeSchema FWOSPlugin TKBool TKCAF TKCDF TKernel TKFeat TKFillet TKG2d TKG3d TKGeomAlgo TKGeomBase TKHLR TKMath TKOffset TKPrim TKPShape TKService TKTopAlgo TKV3d TKMesh TKBO TKXDESTEP TKXCAF TKXCAFSchema TKLCAF TKPLCAF )
#inherits variables from parent dir - don't need to 'find_package ( OCE )' again
if( ${OCE_FOUND} )
  set( OpenCASCADE_LIBRARIES ${CASCADE_LIBS} )
  set( OpenCASCADE_INCLUDE_DIR ${OCE_INCLUDE_DIRS} )
else() #look for OpenCASCADE
  set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
  find_package ( OpenCASCADE REQUIRED COMPONENTS ${CASCADE_LIBS} )
endif()

find_package( OpenGL REQUIRED )
find_package( wxWidgets REQUIRED COMPONENTS adv base core gl aui propgrid )
find_package( PythonLibs REQUIRED )
include(${wxWidgets_USE_FILE})

set( AREA_LIBS area )

include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${wxWidgets_INCLUDE_DIRS} ${OpenCASCADE_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS} . ${OPENGL_INCLUDE_DIR}
    )
set ( EXECUTABLE_OUTPUT_PATH ../bin )

set( heekscad_HDRS
    AboutBox.h
    AutoSave.h
    BezierCurve.h
    ../interface/Box.h
    Cone.h
    ConversionTools.h
    CoordinateSystem.h
    CorrelationTool.h
    Cuboid.h
    CxfFont.h
    Cylinder.h
    Dexel.cpp
    DexelContour.cpp
    DexelGrid.h
    DexelSolid.h
    DigitizedPoint.h
    DigitizeMode.h
    DimensionDrawing.h
    ../interface/DoubleInput.h
    Drawing.h
    dxf.h
    Edge.h
    EndedObject.h
    Face.h
    FaceTools.h
    FastCurves.h
    glfont2.h
    GLList.h
    GraphicsCanvas.h
    Grid.h
    Gripper.h
    GripperSelTransform.h
    Group.h
    HAngularDimension.h
    HArc.h
    HArea.h
    HCircle.h
    ../interface/HDialogs.h
    HDimension.h
    HDxf.h
    HeeksCAD.h
    ../interface/HeeksCADInterface.h
    ../interface/HeeksColor.h
    HeeksFrame.h
    ../interface/HeeksObj.h
    HeeksPrintout.h
    History.h
    HEllipse.h
    HGear.h
    HILine.h
    HImage.h
    HLine.h
    HPoint.h
    HSpline.h
    HText.h
    HXml.h
    Index.h
    ../interface/InputMode.h
    InputModeCanvas.h
    Intersector.h
    ../interface/LeftAndRight.h
    LineArcDrawing.h
    Loop.h
    MagDragWindow.h
    MarkedList.h
    ../interface/MarkedObject.h
    ../interface/Material.h
    ../interface/NiceTextCtrl.h
    ../interface/ObjList.h
    ObjPropsCanvas.h
    ../interface/Observer.h
    OptionsCanvas.h
    OrientationModifier.h
    ../interface/PictureFrame.h
    Plugins.h
    PointDrawing.h
    PointOrWindow.h
    PropBinding.h
    PropBindingDialog.h
    PropertiesCanvas.h
    PropertyChange.h
    ../interface/Property.h
    ../interface/PropertyList.h
    Pyramid.h
    RegularShapesDrawing.h
    RemoveOrAddTool.h
    RuledSurface.h
    Ruler.h
    Sectioning.h
    SelectMode.h
    Shape.h
    ShapeData.h
    ShapeTools.h
    Sketch.h
    Solid.h
    SolidTools.h
    Sphere.h
    stdafx.h
    StlSolid.h
    ../interface/strconv.h
    StretchTool.h
    svg.h
    ../tinyxml/tinystr.h
    ../tinyxml/tinyxml.h
    ../interface/Tool.h
    ../interface/ToolImage.h
    ../interface/ToolList.h
    TransformTool.h
    TransformTools.h
    TreeCanvas.h
    Vertex.h
    ViewPanning.h
    ViewPoint.h
    ViewRotating.h
    ViewZooming.h
    Window.h
    Wire.h
    wxImageLoader.h
   )

set( heekscad_SRCS
    AboutBox.cpp
    AutoSave.cpp
    BezierCurve.cpp
    Cone.cpp
    ConversionTools.cpp
    CoordinateSystem.cpp
    CorrelationTool.cpp
    Cuboid.cpp
    CxfFont.cpp
    Cylinder.cpp
    Dexel.cpp
    DexelContour.cpp
    DexelGrid.cpp
    DexelSolid.cpp
    DigitizedPoint.cpp
    DigitizeMode.cpp
    DimensionDrawing.cpp
    Drawing.cpp
    dxf.cpp
    Edge.cpp
    EndedObject.cpp
    ExitMainLoop.cpp
    Face.cpp
    FaceTools.cpp
    FastCurves.cpp
    glfont2.cpp
    GLList.cpp
    GraphicsCanvas.cpp
    Grid.cpp
    Gripper.cpp
    GripperSelTransform.cpp
    Group.cpp
    HAngularDimension.cpp
    HArc.cpp
    HArea.cpp
    HCircle.cpp
    HDimension.cpp
    HDxf.cpp
    HeeksCAD.cpp
    HeeksFrame.cpp
    HeeksPrintout.cpp
    History.cpp
    HEllipse.cpp
    HGear.cpp
    HILine.cpp
    HImage.cpp
    HLine.cpp
    HPoint.cpp
    HSpline.cpp
    HText.cpp
    HXml.cpp
    Input.cpp
    InputModeCanvas.cpp
    LineArcDrawing.cpp
    Loop.cpp
    MagDragWindow.cpp
    MarkedList.cpp
    ObjPropsCanvas.cpp
    OptionsCanvas.cpp
    OrientationModifier.cpp
    Plugins.cpp
    PointDrawing.cpp
    PointOrWindow.cpp
    PropBinding.cpp
    PropBindingDialog.cpp
    PropertiesCanvas.cpp
    PropertyChange.cpp
    Pyramid.cpp
    RegularShapesDrawing.cpp
    RemoveOrAddTool.cpp
    RS274X.cpp
    RuledSurface.cpp
    Ruler.cpp
    Sectioning.cpp
    SelectMode.cpp
    Shape.cpp
    ShapeData.cpp
    ShapeTools.cpp
    Sketch.cpp
    Solid.cpp
    SolidTools.cpp
    Sphere.cpp
    stdafx.cpp
    StlSolid.cpp
    StretchTool.cpp
    svg.cpp
    TransformTool.cpp
    TransformTools.cpp
    TreeCanvas.cpp
    Vertex.cpp
    ViewPanning.cpp
    ViewPoint.cpp
    ViewRotating.cpp
    ViewZooming.cpp
    Window.cpp
    Wire.cpp
    wxImageLoader.cpp
   )

set( HeeksCAD_LIBS
       -L../bin -ltinyxml -lheeksintf
   )
add_executable( heekscad ${heekscad_SRCS} ${platform_SRCS} ${heekscad_HDRS} )
# Link ${HeeksCAD_LIBS} first to prevent installed libs from being found first in the search order
target_link_libraries( heekscad
                       ${HeeksCAD_LIBS} ${AREA_LIBS}
                       ${wxWidgets_LIBRARIES}  ${OpenCASCADE_LIBRARIES}
                       ${PYTHON_LIBRARIES} ${OSX_LIBS} ${OPENGL_LIBRARIES} )

set_target_properties( heekscad PROPERTIES VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH} )
install( TARGETS heekscad DESTINATION bin )

file( GLOB bitmaps "${CMAKE_CURRENT_SOURCE_DIR}/../bitmaps/*.png"
                   "${CMAKE_CURRENT_SOURCE_DIR}/../bitmaps/*.svg"
                   "${CMAKE_CURRENT_SOURCE_DIR}/../bitmaps/*.glf"
    )
install( FILES ${bitmaps} DESTINATION share/heekscad/bitmaps )

file( GLOB icons "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*.png" )
install( FILES ${icons} DESTINATION share/heekscad/icons )

file( GLOB menu_icon "${CMAKE_CURRENT_SOURCE_DIR}/../icons/HeeksCAD.svg" )
install( FILES ${menu_icon} DESTINATION share/icons/hicolor/scalable/apps ) # freedesktop.org standard location

file( GLOB desktop "${CMAKE_CURRENT_SOURCE_DIR}/../HeeksCAD.desktop" )
install( FILES ${desktop} DESTINATION share/applications )

