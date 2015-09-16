cmake_minimum_required(VERSION 3.1)
project(cborpp VERSION 1.0.0)

add_library(cborpp INTERFACE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(Dart)

#============================================================================
# Compiler properties
#============================================================================

### Setting compiler properties
target_compile_features(cborpp INTERFACE cxx_auto_type cxx_binary_literals cxx_nullptr cxx_range_for)

target_include_directories(
        cborpp
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

#============================================================================
# Third-party Libraries
#============================================================================

# Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
# /Boost

# Bundled libraries
include_directories(contrib/include)

#============================================================================
# Install
#============================================================================
# * https://github.com/forexample/package-example
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
)

# Note: use 'targets_export_name'
configure_file("cmake/Config.cmake.in" "${project_config}" @ONLY)

install(
        TARGETS cborpp
        EXPORT "${targets_export_name}"
        INCLUDES DESTINATION "${include_install_dir}"
)

install(DIRECTORY "include/cborpp" DESTINATION "${include_install_dir}")

install(
        FILES "${project_config}" "${version_config}"
        DESTINATION "${config_install_dir}"
)

install(
        EXPORT "${targets_export_name}"
        NAMESPACE "${namespace}"
        DESTINATION "${config_install_dir}"
)

#============================================================================
# Documentation generation
#============================================================================

#-- Add an Option to toggle the generation of the API documentation
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if (BUILD_DOCUMENTATION)
    FIND_PACKAGE(Doxygen)
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR
                "Doxygen is needed to build the documentation. Please install it correctly")
    endif ()
    #-- Configure the Template Doxyfile for our specific project
    configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    #-- Add a custom target to run Doxygen when ever the project is built
    add_custom_target(docs ALL COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
    # IF you do NOT want the documentation to be generated EVERY time you build the project
    # then leave out the 'ALL' keyword from the above command.
endif ()

#============================================================================
# Unit testing
#============================================================================
if (BUILD_TESTING)
    add_subdirectory(tests)
endif (BUILD_TESTING)