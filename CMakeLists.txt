cmake_minimum_required(VERSION 3.18)

project(zwo_asi VERSION 0.1)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_C_FLAGS_DEBUG "-g")
#add_compile_options(-ggdb3)

# Common installation directories
include(GNUInstallDirs)

# Use -fPIC even if statically compiled
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake
                      ${CMAKE_MODULE_PATH})


find_package(Python3 COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)
find_package(Python3 COMPONENTS Interpreter)
find_package(USB REQUIRED)
find_package(Threads REQUIRED)



##########################
# armv7 or x64 library ? #
# ZWO library            #
##########################

if(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
  set(LIBASICAMERA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/asi_libs/armv7/libASICamera2.a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  set(LIBASICAMERA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/asi_libs/armv8/libASICamera2.a)
else()
  set(LIBASICAMERA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/asi_libs/x64/libASICamera2.a)
endif()


###################################################
# zwo_asi: library for higher level calls to the #
# ZWO library                                     #
###################################################

add_library(zwo_asi
  src/utils.cpp
  src/camera_exception.cpp
  src/controllable.cpp
  src/controllable_exception.cpp
  src/camera_info.cpp
  src/image_type.cpp
  src/bayer_pattern.cpp
  src/roi.cpp
  src/roi_exception.cpp
  src/camera_mode.cpp
  src/guide_direction.cpp
  src/camera.cpp
  src/VideoThread.cpp
  )

add_library(zwo_asi::zwo_asi ALIAS zwo_asi)

target_include_directories(zwo_asi PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
   ${ASI_INCLUDE_DIR}
   )
 #target_include_directories(Tracker PUBLIC
   #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/tracking>
  # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# )
 target_link_libraries(zwo_asi ${LIBASICAMERA_PATH} ${LIBUSB_LIBRARIES} Threads::Threads)
 #target_link_libraries(Tracker Threads::Threads m zwo_asi) ##ADDED MR

#################################
# python bindings over zwo_asi #
#################################

if(CALL_FROM_SETUP_PY)
    set(ASI_CAMS_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
else()
    set(ASI_CAMS_INSTALL_PREFIX ${Python3_SITELIB})
endif()
pybind11_add_module(pybind11_bindings MODULE srcpy/bindings.cpp)
set_target_properties(pybind11_bindings PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/zwo_asi
  OUTPUT_NAME "bindings"
  )
target_link_libraries(pybind11_bindings
  PUBLIC  zwo_asi::zwo_asi ${LIBASICAMERA_PATH}  #MODIFIED MR
  )

###########
# install #
###########

install(
    TARGETS zwo_asi  #MODIFIED MR
    EXPORT zwo_asiTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

install(
    TARGETS pybind11_bindings
    COMPONENT bindings
    LIBRARY DESTINATION ${ASI_CAMS_INSTALL_PREFIX}
    ARCHIVE DESTINATION ${ASI_CAMS_INSTALL_PREFIX}
    RUNTIME DESTINATION ${ASI_CAMS_INSTALL_PREFIX}
    )
