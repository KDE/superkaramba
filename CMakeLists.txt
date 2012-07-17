PROJECT(superkaramba)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    find_package(KDE4)
    include(KDE4Defaults)
    set(IMAGEBLITZ_REQUIRED TRUE)

    add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
    add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

    # If definitions like -D_GNU_SOURCE are needed for these checks they
    # should be added to _KDE4_PLATFORM_DEFINITIONS when it is originally
    # defined outside this file.  Here we include these definitions in
    # CMAKE_REQUIRED_DEFINITIONS so they will be included in the build of
    # checks below.
    set( CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS} )
else()
    set(IMAGEBLITZ_REQUIRED FALSE)
endif()
macro_optional_find_package( QImageBlitz )
macro_log_feature( QIMAGEBLITZ_FOUND "QImageBlitz" "An image effects library" "kdesupport" ${IMAGEBLITZ_REQUIRED} "" "Required for building SuperKaramba.")

macro_optional_find_package(PythonLibs 2)
#Laurent doesn't compile for the moment
remove_definitions( -DQT_NO_CAST_FROM_ASCII )
remove_definitions( -DQT_NO_CAST_TO_ASCII )



if(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
  SET(PYTHONLIBS_FOUND ON)
endif(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)

macro_log_feature(PYTHONLIBS_FOUND "Python Libraries" "The Python library is required to build the legacy SuperKaramba bindings for Python" "http://www.python.org")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    macro_display_feature_log()
else()
    if(NOT QIMAGEBLITZ_FOUND)
        return()
    endif()
endif()

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_INCLUDES}
  ${KDE4_INCLUDE_DIR}
  ${KDE4_INCLUDES}
)

if(PYTHONLIBS_FOUND)
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
endif(PYTHONLIBS_FOUND)

ADD_SUBDIRECTORY( src )
macro_optional_add_subdirectory( icons )

macro_optional_add_subdirectory( plasma )
