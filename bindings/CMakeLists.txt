include(FindSWIG)
include(FindPythonLibs)
include(UseSWIG)

if(SWIG_FOUND)
if(PYTHONLIBS_FOUND)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../src)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

    SWIG_ADD_MODULE(ftdi python ftdi.i)
    SWIG_LINK_LIBRARIES(ftdi ${PYTHON_LIBRARIES})

    message(STATUS "Building python bindings via swig")
endif(PYTHONLIBS_FOUND)
endif(SWIG_FOUND)
