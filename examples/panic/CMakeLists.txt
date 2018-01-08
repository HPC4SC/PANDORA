set( EX_NAME panic )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( NAMESPCACE Panic )
set( FILE_NAME PanicAgent )
add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx mpiCode/${FILE_NAME}_mpi.cxx
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.* .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py ${NAMESPCACE} ${FILE_NAME}
    DEPENDS main.cxx ${FILE_NAME}.cxx ${FILE_NAME}.hxx
)

# add the executable
add_executable( ${EX_NAME}
                main.cxx
                MoveAction.cxx
                PanicAgent.cxx
                ScenarioConfig.cxx
                Scenario.cxx
                mpiCode/FactoryCode.cxx
                mpiCode/${FILE_NAME}_mpi.cxx
                )

target_link_libraries( ${EX_NAME} pandora-shared )
set( XMLFILES config_catedral.xml config_cat.xml config_concert.xml config_humanWall.xml )
install( TARGETS    ${EX_NAME}          DESTINATION examples/${EX_NAME} )
install( FILES      README ${XMLFILES}  DESTINATION examples/${EX_NAME} )
install( DIRECTORY  resources           DESTINATION examples/${EX_NAME} )
