add_library(mikaireader SHARED reader.c)
target_include_directories(mikaireader PRIVATE ..)

# Link libnfc library
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNFC REQUIRED libnfc)
target_include_directories(mikaireader PRIVATE ${LIBNFC_INCLUDE_DIRS})
target_link_directories(mikaireader PRIVATE ${LIBNFC_LIBRARY_DIRS})
target_link_libraries(mikaireader ${LIBNFC_LIBRARIES})