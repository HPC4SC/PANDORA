# Install script for directory: /home/quimlaz/PANDORA-cmakedocu/examples/randomWalkers/analysis

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/opt/pandora")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal"
         RPATH "/opt/hdf5-1.8.19/lib:/opt/gdal-1.10.1/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal" TYPE EXECUTABLE FILES "/home/quimlaz/PANDORA-cmakedocu/build/examples/randomWalkers/analysis/analysisRanWal")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal"
         OLD_RPATH "/home/quimlaz/PANDORA-cmakedocu/build/src:/home/quimlaz/PANDORA-cmakedocu/build/utils:/opt/hdf5-1.8.19/lib:/opt/gdal-1.10.1/lib:/home/quimlaz/PANDORA-cmakedocu/build/tinyxml:"
         NEW_RPATH "/opt/hdf5-1.8.19/lib:/opt/gdal-1.10.1/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/examples/randomWalkers/analysisRanWal/analysisRanWal")
    endif()
  endif()
endif()

