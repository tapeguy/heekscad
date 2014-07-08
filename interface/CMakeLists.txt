project( heeksintf )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall -Wno-overloaded-virtual -DHEEKSCAD -DUNICODE -DTIXML_USE_STL )
FIND_PACKAGE( wxWidgets REQUIRED COMPONENTS base core gl )
INCLUDE(${wxWidgets_USE_FILE})
include_directories ( ${CMAKE_SOURCE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR}
                      ${wxWidgets_INCLUDE_DIRS}
                      ../src
                    )
set ( LIBRARY_OUTPUT_PATH ../bin )

set( heeksintf_SRCS
    DoubleInput.cpp
    Geom.cpp
    GripperTypes.cpp
    HDialogs.cpp
    HeeksCADInterface.cpp
    HeeksColor.cpp
    HeeksConfig.cpp
    HeeksObj.cpp
    LeftAndRight.cpp
    MarkedObject.cpp
    NiceTextCtrl.cpp
    ObjList.cpp
    ObjTypes.cpp
    ObjectCanvas.cpp
    PictureFrame.cpp
    Property.cpp
    PropertyList.cpp
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
    HeeksConfig.h
    HeeksObj.h
    InputMode.h
    LeftAndRight.h
    MarkedObject.h
    Material.h
    NiceTextCtrl.h
    NurbSurfaceParams.h
    ObjList.h
    ObjTypes.h
    ObjectCanvas.h
    Observer.h
    PictureFrame.h
    Plugin.h
    Property.h
    PropertyList.h
    SketchOrder.h
    Tool.h
    ToolImage.h
    ToolList.h
    TransientObject.h
    strconv.h
   )

add_library( heeksintf SHARED ${heeksintf_SRCS} ${heeksintf_HDRS} )
target_link_libraries( heeksintf ${wxWidgets_LIBRARIES} )
set_target_properties( heeksintf PROPERTIES SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH} )
install( TARGETS heeksintf DESTINATION lib )
