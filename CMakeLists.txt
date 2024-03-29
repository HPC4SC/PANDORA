cmake_minimum_required (VERSION 3.1)
project( Pandora )

if (NOT debug)          # If variable not passed to cmake (-D debug=[true|false])
    set(debug false) 
endif ()
if (NOT edebug)         # If variable not passed to cmake (-D edebug=[true|false])
    set(edebug false)
endif ()

message("MODE DEBUG: " ${debug} ". MODE EXTREME_DEBUG: " ${edebug})

if (debug) 
    if (edebug)
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DGDAL_VERSION_1 -DTIXML_USE_STL -g -DPANDORAEDEBUG" )
    else ()
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DGDAL_VERSION_1 -DTIXML_USE_STL -g -DPANDORADEBUG" )
    endif ()
else ()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DGDAL_VERSION_1 -DTIXML_USE_STL -g" )
endif()
#  -Wl,-z,defs  -DPANDORADEBUG
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g" )

# PYTHON
find_package( PythonInterp REQUIRED )
find_package( PythonLibs REQUIRED )
include_directories( src "${PYTHON_INCLUDE_DIR}" )

# BOOST
set( BSC_MACHINE $ENV{BSC_MACHINE} )
if (BSC_MACHINE)
    set( BOOST_PYTHON_STRING "python" )
else( )
    set( BOOST_PYTHON_STRING "python-py${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}" )
endif()

find_package( Boost COMPONENTS filesystem system timer ${BOOST_PYTHON_STRING} REQUIRED )

# User should set GDAL_DIR or GDAL_ROOT if necessary
find_package( GDAL REQUIRED )

EXEC_PROGRAM( ${GDAL_CONFIG} ARGS --version OUTPUT_VARIABLE GDAL_VERSION )
if( GDAL_VERSION VERSION_GREATER "1.11.4" )
    message( FATAL_ERROR "Wrong GDAL version. (Needed <= 1.11.4) (Current " ${GDAL_VERSION} ")" )
endif()
include_directories( "${GDAL_INCLUDE_DIR}" ) # GDAL_FOUND / GDAL_LIBRARY

# User should set HDF5_ROOT if necessary
find_package( HDF5 REQUIRED )
if( HDF5_VERSION VERSION_GREATER "1.8.19" )
    message( FATAL_ERROR "Wrong HDF5 version. (Needed <= 1.8.19) (Current " ${HDF5_VERSION} ")" )
endif()
include_directories( "${HDF5_INCLUDE_DIRS}" )

find_package(MPI)
include_directories( ${MPI_INCLUDE_PATH} )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

add_subdirectory( tinyxml )
include_directories( ${CMAKE_SOURCE_DIR}/tinyxml )

add_subdirectory( src )
add_subdirectory( utils )
add_subdirectory( pyPandora )
#add_subdirectory( examples )
add_subdirectory( bin )

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
