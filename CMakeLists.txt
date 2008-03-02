PROJECT(superkaramba)

macro_optional_find_package(PythonLibs)
find_package(Plasma)

if(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
  SET(PYTHONLIBS_FOUND ON)
endif(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
macro_log_feature(PYTHONLIBS_FOUND "Python Libraries" "The Python library is required to build the legacy SuperKaramba bindings for Python" "http://www.python.org")

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_INCLUDES}
  ${KDE4_INCLUDE_DIR}
)

if(PYTHONLIBS_FOUND)
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
endif(PYTHONLIBS_FOUND)

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( icons )

if(PLASMA_FOUND)
  ADD_SUBDIRECTORY( plasma )
endif(PLASMA_FOUND)
