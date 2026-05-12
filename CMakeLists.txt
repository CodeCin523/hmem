cmake_minimum_required(VERSION 3.11)

# SETUP PROJECT
project(hmem
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SETUP LIBRARY
add_library(hmem INTERFACE)
add_library(hmem::hmem ALIAS hmem)

set_target_properties(
    hmem PROPERTIES
        VERSION     ${PROJECT_VERSION}
        SOVERSION   ${PROJECT_VERSION_MAJOR}
)

target_include_directories(hmem
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(hmem INTERFACE c_std_11)

# INSTALL CONFIGURATION
include(GNUInstallDirs)

install(
    TARGETS hmem
    EXPORT hmemTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# PACKAGE CONFIG
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "hmemConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/hmemConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/hmemConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/hmem
)

install(
    EXPORT hmemTargets
    FILE hmemTargets.cmake
    NAMESPACE hmem::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/hmem
)

# TESTS
if(HMEM_BUILD_TESTS)
    add_subdirectory(tests)
endif()