PROJECT(superkaramba)

find_package( KDE4 REQUIRED )
find_package( Blitz REQUIRED )
include( KDE4Defaults )
include( MacroLibrary )
include( MacroOptionalAddSubdirectory )

macro_optional_find_package(PythonLibs)
macro_optional_find_package(Plasma)

if(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
  SET(PYTHONLIBS_FOUND ON)
endif(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)

macro_log_feature(PYTHONLIBS_FOUND "Python Libraries" "The Python library is required to build the legacy SuperKaramba bindings for Python" "http://www.python.org")

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

if(PLASMA_FOUND)
 macro_optional_add_subdirectory( plasma )
endif(PLASMA_FOUND)
