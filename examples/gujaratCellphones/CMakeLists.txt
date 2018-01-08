set( EX_NAME gujaratCellphones )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} libmdp )

set( NAMESPCACE GujaratCellphones )
set( FILE_NAME  Herder )
set( FILE_NAME2 Village )
add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx mpiCode/${FILE_NAME}_mpi.cxx mpiCode/${FILE_NAME2}_mpi.cxx
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.* .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME2}.* .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py ${NAMESPCACE} ${FILE_NAME} ${FILE_NAME2}
    DEPENDS main.cxx ${FILE_NAME}.cxx ${FILE_NAME}.hxx ${FILE_NAME2}.cxx ${FILE_NAME2}.hxx 
)

# add the executable
add_executable( ${EX_NAME}
                Climate.cxx
                DecisionModel.cxx
                Herder.cxx
                HerderState.cxx
                HerderWorldConfig.cxx
                HerderWorld.cxx
                main.cxx
                MoveAction.cxx
                Village.cxx
                mpiCode/FactoryCode.cxx
                mpiCode/${FILE_NAME}_mpi.cxx
                mpiCode/${FILE_NAME2}_mpi.cxx
                )


target_link_libraries( ${EX_NAME} pandora-shared )
install( TARGETS    ${EX_NAME} DESTINATION  examples/${EX_NAME} )
install( FILES      config.xml DESTINATION  examples/${EX_NAME} )
