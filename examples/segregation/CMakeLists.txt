set( EX_NAME segregation )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( NAMESPACE Segregation )
set( FILE_NAME Neighbor )
add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx mpiCode/${FILE_NAME}_mpi.cxx
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.* .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py ${NAMESPACE} ${FILE_NAME}
    DEPENDS main.cxx ${FILE_NAME}.cxx ${FILE_NAME}.hxx
)

# add the executable
add_executable( ${EX_NAME}
                main.cxx
                NeighborConfig.cxx
                Neighbor.cxx
                Neighborhood.cxx
                mpiCode/FactoryCode.cxx
                mpiCode/${FILE_NAME}_mpi.cxx
                )
target_link_libraries( ${EX_NAME} pandora-shared )

install( TARGETS    ${EX_NAME}          DESTINATION examples/${EX_NAME} )
install( FILES      config.xml README   DESTINATION examples/${EX_NAME} )
