project( heeksintf )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall -DHEEKSCAD -DUNICODE -DTIXML_USE_STL )
FIND_PACKAGE( wxWidgets REQUIRED COMPONENTS base core gl )
INCLUDE(${wxWidgets_USE_FILE})
find_package( PythonLibs REQUIRED )
include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                      ${PYTHON_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS}
                      ../src
                    )
set ( LIBRARY_OUTPUT_PATH ../bin )

set( heeksintf_SRCS
    DoubleInput.cpp
    Geom.cpp
    HDialogs.cpp
    HeeksCADInterface.cpp
    HeeksColor.cpp
    HeeksObj.cpp
    LeftAndRight.cpp
    MarkedObject.cpp
    NiceTextCtrl.cpp
    ObjList.cpp
    ObjectCanvas.cpp
    PictureFrame.cpp
    PropertyCheck.cpp
    PropertyChoice.cpp
    PropertyColor.cpp
    PropertyDouble.cpp
    PropertyFile.cpp
    PropertyInt.cpp
    PropertyLength.cpp
    PropertyList.cpp
    PropertyString.cpp
    PropertyVertex.cpp
    ToolImage.cpp
    ToolList.cpp
    TransientObject.cpp
    strconv.cpp
   )
   
set( heeksintf_HDRS
    Box.h
    DoubleInput.h
    Geom.h
    GripData.h
    GripperTypes.h
    HDialogs.h
    HeeksCADInterface.h
    HeeksColor.h
    HeeksObj.h
    InputMode.h
    LeftAndRight.h
    MarkedObject.h
    Material.h
    NiceTextCtrl.h
    NurbSurfaceParams.h
    ObjList.h
    ObjectCanvas.h
    Observer.h
    PictureFrame.h
    Plugin.h
    Property.h
    PropertyCheck.h
    PropertyChoice.h
    PropertyColor.h
    PropertyDouble.h
    PropertyFile.h
    PropertyInt.h
    PropertyLength.h
    PropertyList.h
    PropertyString.h
    PropertyVertex.h
    SketchOrder.h
    Tool.h
    ToolImage.h
    ToolList.h
    TransientObject.h
    strconv.h
   )

add_library( heeksintf SHARED ${heeksintf_SRCS} ${heeksintf_HDRS} )
target_link_libraries( heeksintf ${PYTHON_LIBRARIES} ${wxWidgets_LIBRARIES} )
set_target_properties( heeksintf PROPERTIES SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH} )
install( TARGETS heeksintf DESTINATION lib )
