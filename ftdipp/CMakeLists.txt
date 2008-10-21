# Check
set(FTDI_BUILD_CPP False PARENT_SCOPE)

# Includes
include_directories( ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../src
                     )

# Targets
set(cpp_sources   ftdi.cpp)
set(cpp_headers   ftdi.hpp)

# Find Boost
find_package(Boost)
if(Boost_FOUND)
set(FTDI_BUILD_CPP True PARENT_SCOPE)
message(STATUS "Building libftdi++")

# Targets
add_library(ftdi_cpp SHARED ${cpp_sources})

# Dependencies
target_link_libraries(ftdi_cpp ftdi usb ${BOOST_LIBRARIES})

# Install
if(${UNIX})

   install( TARGETS ftdi_cpp
            LIBRARY DESTINATION lib
            COMPONENT sharedlibs
            )

   install( FILES ${cpp_headers}
            DESTINATION include/${PROJECT_NAME}
            COMPONENT headers
            )

endif(${UNIX})

if(${WIN32})

   install( TARGETS ftdi_cpp
            DESTINATION bin
            COMPONENT sharedlibs
            )

   install( FILES ${cpp_headers}
            DESTINATION include/${PROJECT_NAME}
            COMPONENT headers
            )

endif(${WIN32})

else(Boost_FOUND)
   message(STATUS "Boost not found, won't build libftdi++")
endif(Boost_FOUND)
