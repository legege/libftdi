# Includes
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     )

# Version information
set(SNAPSHOT_VERSION "unknown")
execute_process(COMMAND git describe
                OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT
                RESULT_VARIABLE GIT_DESCRIBE_RESULT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )
if(${GIT_DESCRIBE_RESULT} STREQUAL 0)
    set(SNAPSHOT_VERSION ${GIT_DESCRIBE_OUTPUT})
endif(${GIT_DESCRIBE_RESULT} STREQUAL 0)
message(STATUS "Detected git snapshot version: ${SNAPSHOT_VERSION}")

configure_file(ftdi_version_i.h.in "${CMAKE_CURRENT_BINARY_DIR}/ftdi_version_i.h" @ONLY)

# Targets
set(c_sources     ftdi.c ftdi_stream.c)
set(c_headers     ftdi.h)

add_library(ftdi SHARED ${c_sources})

math(EXPR VERSION_FIXUP "${MAJOR_VERSION} + 1")    # Compatiblity with previous releases
set_target_properties(ftdi PROPERTIES VERSION ${VERSION_FIXUP}.${MINOR_VERSION}.0 SOVERSION 2)

# Static library
add_library(ftdi-static STATIC ${c_sources})
set_target_properties(ftdi-static PROPERTIES OUTPUT_NAME "ftdi")

# Prevent clobbering each other during the build
set_target_properties(ftdi PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ftdi-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# Dependencies
target_link_libraries(ftdi ${LIBUSB_LIBRARIES})

# Install
if(${UNIX})

   install( TARGETS ftdi
            LIBRARY DESTINATION lib${LIB_SUFFIX}
            COMPONENT sharedlibs
            )

   install( TARGETS ftdi-static
            ARCHIVE DESTINATION lib${LIB_SUFFIX}
            COMPONENT staticlibs
            )

   install( FILES ${c_headers}
            DESTINATION include/${PROJECT_NAME}
            COMPONENT headers
            )

endif(${UNIX})

if(${WIN32})

   install( TARGETS ftdi
            DESTINATION bin
            COMPONENT sharedlibs
            )

   install( TARGETS ftdi-static
            DESTINATION bin
            COMPONENT staticlibs
            )

   install( FILES ${c_headers}
            DESTINATION include/${PROJECT_NAME}
            COMPONENT headers
            )

endif(${WIN32})
