include(FindSWIG)
include(FindPythonLibs)
include(FindPythonInterp)
include(UseSWIG)

option(PYTHON_BINDINGS "Build python bindings via swig" ON)

if(PYTHON_BINDINGS AND SWIG_FOUND AND PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
    INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
    LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../src)

    SWIG_ADD_MODULE(ftdi python ftdi.i)
    SWIG_LINK_LIBRARIES(ftdi ftdi ${PYTHON_LIBRARIES})

    execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print '%d.%d' % ( sys.version_info[0], sys.version_info[1] )"
                      OUTPUT_VARIABLE PYTHON_VERSION
                      OUTPUT_STRIP_TRAILING_WHITESPACE )

    set ( SITEPACKAGE lib${LIB_SUFFIX}/python${PYTHON_VERSION}/site-packages )

    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/_ftdi.so DESTINATION ${SITEPACKAGE})
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/ftdi.py DESTINATION ${SITEPACKAGE})

    message(STATUS "Building python bindings via swig. Will be installed under ${CMAKE_INSTALL_PREFIX}/${SITEPACKAGE}")
else(PYTHON_BINDINGS AND SWIG_FOUND AND PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
    message(STATUS "Not building python bindings")
endif(PYTHON_BINDINGS AND SWIG_FOUND AND PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
