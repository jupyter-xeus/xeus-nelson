#############################################################################
#Copyright (c) 2023,                                          
#                                                                         
#Distributed under the terms of the BSD 3-Clause License.                 
#                                                                         
#The full license is in the file LICENSE, distributed with this software. 
#############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-nelson)

set(XEUS_NELSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XEUS_NELSON_INCLUDE_DIR}/xeus-nelson/xeus_nelson_config.hpp" version_defines
     REGEX "#define XEUS_NELSON_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${version_defines})
    if (ver MATCHES "#define XEUS_NELSON_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_NELSON_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${XEUS_NELSON_VERSION_MAJOR}.${XEUS_NELSON_VERSION_MINOR}.${XEUS_NELSON_VERSION_PATCH})
message(STATUS "Building xeus-nelson v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED XEUS_NELSON_KERNELSPEC_PATH)
    set(XEUS_NELSON_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xnelson/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xnelson/kernel.json"
)

# Build options
# =============

option(XEUS_NELSON_BUILD_STATIC "Build xeus-nelson static library" ON)
option(XEUS_NELSON_BUILD_SHARED "Split xnelson build into executable and library" ON)
option(XEUS_NELSON_BUILD_EXECUTABLE "Build the xnelson executable" ON)

option(XEUS_NELSON_USE_SHARED_XEUS "Link xnelson  with the xeus shared library (instead of the static library)" ON)
option(XEUS_NELSON_USE_SHARED_XEUS_NELSON "Link xnelson  with the xeus-nelson shared library (instead of the static library)" ON)
option(XEUS_NELSON_EMSCRIPTEN_WASM_BUILD "Build for wasm with emscripten" OFF)

if (XEUS_NELSON_EMSCRIPTEN_WASM_BUILD)
    message(STATUS "Building with emscripten")
    set(XEUS_NELSON_BUILD_STATIC ON)
    set(XEUS_NELSON_BUILD_SHARED OFF)
    set(XEUS_NELSON_BUILD_EXECUTABLE OFF)
    set(XEUS_NELSON_USE_SHARED_XEUS OFF)
    set(XEUS_NELSON_USE_SHARED_XEUS_NELSON OFF)
endif ()

# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.7.0)
set(xeus_REQUIRED_VERSION 3.0.5)

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
endif ()

find_package(nelson)

# Flags
# =====
include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if (NOT XEUS_NELSON_EMSCRIPTEN_WASM_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif ()

    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
    if (HAS_CPP_17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++17 support!")
    endif ()
endif ()

# Source files
# ============

set(XEUS_NELSON_HEADERS
    include/xeus-nelson/xeus_nelson_config.hpp
    include/xeus-nelson/xinterpreter.hpp
    include/xeus-nelson/xutils.hpp
)

set(XEUS_NELSON_SRC
    src/xinterpreter.cpp
)

set(XEUS_NELSON_MAIN_SRC
    src/main.cpp
)

# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(xnelson_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # C++17 flag
        CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
        if (HAS_CPP_17_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_17)
        else ()
            message(FATAL_ERROR "Unsupported compiler -- xeus-nelson requires C++17 support!")
        endif ()
    endif ()

    if (APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_RPATH ON
        )
    else ()
        set_target_properties(${target_name} PROPERTIES
            BUILD_WITH_INSTALL_RPATH 1
            SKIP_BUILD_RPATH FALSE
        )
    endif ()

    set_target_properties(${target_name} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endmacro()

# Common macro kernels (xnelson )
macro(xnelson_set_kernel_options target_name)
    if (XEUS_NELSON_USE_SHARED_XEUS_NELSON)
        target_link_libraries(${target_name} PRIVATE xeus-nelson)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_compile_definitions(${target_name} PUBLIC "XEUS_NELSON_STATIC_LIB")
        target_link_libraries(${target_name} PRIVATE xeus-nelson-static)
    endif()

    if (NOT XEUS_NELSON_EMSCRIPTEN_WASM_BUILD)
        find_package(Threads) 
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif ()
endmacro()

# Common macro for shared and static library
macro(xnelson_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${XEUS_NELSON_SRC} ${XEUS_NELSON_HEADERS})
    xnelson_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XEUS_NELSON_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${XEUS_NELSON_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "XEUS_NELSON_EXPORTS")

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               $<BUILD_INTERFACE:${XEUS_NELSON_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if (XEUS_NELSON_USE_SHARED_XEUS)
        set(XEUS_NELSON_XEUS_TARGET xeus)
    else ()
        set(XEUS_NELSON_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(
        ${target_name}
        PUBLIC 
        ${XEUS_NELSON_XEUS_TARGET}
        xtl
        nlsCore
        nlsEngine
        nlsText_completion
        nlsFunctions_manager_builtin)

    if (WIN32 OR CYGWIN)
        #
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()

    if (NOT XEUS_NELSON_EMSCRIPTEN_WASM_BUILD)
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif ()
endmacro()

# xeus-nelson
# ===========

set(XEUS_NELSON_TARGETS "")

if (XEUS_NELSON_BUILD_SHARED)
    # Build libraries
    xnelson_create_target(xeus-nelson SHARED xeus-nelson)
    list(APPEND XEUS_NELSON_TARGETS xeus-nelson)
endif ()

if (XEUS_NELSON_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xnelson_create_target(xeus-nelson-static STATIC xeus-nelson-static)
    else ()
        xnelson_create_target(xeus-nelson-static STATIC xeus-nelson)
    endif ()
    list(APPEND XEUS_NELSON_TARGETS xeus-nelson-static)
    
endif ()

# xnelson
# =======

if (XEUS_NELSON_BUILD_EXECUTABLE)
    find_package(xeus-zmq 1.0.2 REQUIRED)
    add_executable(xnelson ${XEUS_NELSON_MAIN_SRC})
    target_compile_features(xnelson PRIVATE cxx_std_17)
    xnelson_set_common_options(xnelson)
    xnelson_set_kernel_options(xnelson)
    target_link_libraries(xnelson PRIVATE xeus-zmq)
endif()

if (XEUS_NELSON_EMSCRIPTEN_WASM_BUILD)
    include(WasmBuildOptions)
    find_package(xeus-lite REQUIRED)
    add_executable(xnelson_wasm src/main_emscripten_kernel.cpp)
    target_link_libraries(xnelson_wasm PRIVATE xeus-lite)
    target_compile_features(xnelson_wasm PRIVATE cxx_std_17)
    xnelson_set_kernel_options(xnelson_wasm)
    xeus_wasm_compile_options(xnelson_wasm)
    xeus_wasm_link_options(xnelson_wasm "web,worker")
endif ()

# Installation
# ============
include(CMakePackageConfigHelpers)

set(XEUS_NELSON_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-nelsonConfig.cmake")

# Install xeus-nelson and xeus-nelson-static
if (XEUS_NELSON_BUILD_SHARED)
    install(TARGETS ${XEUS_NELSON_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-nelson)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install xnelson
if (XEUS_NELSON_BUILD_EXECUTABLE)
    install(TARGETS xnelson
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-nelson
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xnelson Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-nelsonConfig.cmake' for a build tree
set(XEUS_NELSON_CONFIG_CODE "####### Expanded from \@XEUS_NELSON_CONFIG_CODE\@ #######\n")
set(XEUS_NELSON_CONFIG_CODE "${XEUS_NELSON_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_NELSON_CONFIG_CODE "${XEUS_NELSON_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeus-nelsonConfig.cmake.in for an install tree
set(XEUS_NELSON_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_NELSON_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_NELSON_CMAKECONFIG_INSTALL_DIR})

if (XEUS_NELSON_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${XEUS_NELSON_CMAKECONFIG_INSTALL_DIR})
endif ()

if(XEUS_NELSON_EMSCRIPTEN_WASM_BUILD)
    install(TARGETS xnelson_wasm
            ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(FILES
            "$<TARGET_FILE_DIR:xnelson_wasm>/xnelson_wasm.js"
            "$<TARGET_FILE_DIR:xnelson_wasm>/xnelson_wasm.wasm"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
