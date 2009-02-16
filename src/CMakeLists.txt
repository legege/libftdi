# Includes
include_directories( ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     )

# Targets
set(c_sources     ftdi.c)
set(c_headers     ftdi.h)

add_library(ftdi SHARED ${c_sources})

# Dependencies
target_link_libraries(ftdi ${LIBUSB_LIBRARIES})

# Install
if(${UNIX})

   install( TARGETS ftdi
            LIBRARY DESTINATION lib
            COMPONENT sharedlibs
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

   install( FILES ${c_headers}
            DESTINATION include/${PROJECT_NAME}
            COMPONENT headers
            )

endif(${WIN32})
