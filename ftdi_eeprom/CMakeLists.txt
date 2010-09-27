set(FTDI_BUILD_EEPROM False PARENT_SCOPE)

option(FTDI_EEPROM "Build ftdi_eeprom" ON)

if (FTDI_EEPROM)
    include(FindPkgConfig)
    pkg_check_modules(Confuse libconfuse)
    INCLUDE_DIRECTORIES(${Confuse_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Confuse_LIBRARY_DIRS})
    SET(libs ${libs} ${Confuse_LIBRARIES})

    if(Confuse_FOUND)
        set(FTDI_BUILD_EEPROM True PARENT_SCOPE)
        message(STATUS "Building ftdi_eeprom")

        # TODO: Add real ftdi_eeprom source + binary

    else(Confuse_FOUND)
        message(STATUS "libConfuse not found, won't build ftdi_eeprom")
    endif(Confuse_FOUND)

else(FTDI_EEPROM)

    message(STATUS "ftdi_eeprom build is disabled")

endif(FTDI_EEPROM)
