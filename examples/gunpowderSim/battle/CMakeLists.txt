set( EX_NAME battle )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( NAMESPACE BattleSim )
set( FILE_NAME  Bullet )
set( FILE_NAME2 Soldier )
add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx mpiCode/${FILE_NAME}_mpi.cxx mpiCode/${FILE_NAME2}_mpi.cxx 
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.* .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME2}.* .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Battlefield.hxx .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py ${NAMESPACE} ${FILE_NAME} ${FILE_NAME2}
    DEPENDS main.cxx ${FILE_NAME}.cxx ${FILE_NAME}.hxx ${FILE_NAME2}.cxx ${FILE_NAME2}.hxx 
)

# add the executable
add_executable( ${EX_NAME}
                BattlefieldConfig.cxx
                Battlefield.cxx
                Bullet.cxx
                main.cxx
                Soldier.cxx
                mpiCode/FactoryCode.cxx
                mpiCode/${FILE_NAME}_mpi.cxx
                mpiCode/${FILE_NAME2}_mpi.cxx )
target_link_libraries( ${EX_NAME} pandora-shared )

#add_subdirectory( analysis )

install( TARGETS    ${EX_NAME} DESTINATION examples/gunpowderSim/${EX_NAME} )
install( FILES      config.xml DESTINATION examples/gunpowderSim/${EX_NAME} )
