# Compile project files
add_library(${LIBRARY_NAME} SHARED
        mikai.c
        srix/srix.c
        srix/srix-flag.c
        mykey/mykey.c
)

# Include mikai private headers
target_include_directories(${LIBRARY_NAME} PRIVATE srix)
target_include_directories(${LIBRARY_NAME} PRIVATE .)

# Link mikaireader library
add_subdirectory(reader)
target_link_libraries(${LIBRARY_NAME} mikaireader)

# Compiled library folder
if (DEFINED CMAKE_INSTALL_LIBDIR)
    SET(libdir ${CMAKE_INSTALL_LIBDIR})
else (DEFINED CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR lib)
    SET(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endif (DEFINED CMAKE_INSTALL_LIBDIR)

# Install compiled mikai library and mikaireader
INSTALL(TARGETS mikai LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL(TARGETS mikaireader LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})