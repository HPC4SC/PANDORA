set( EX_NAME gujarat )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} libmdp )

set( NAMESPACE Gujarat )
set( FILE_NAME HunterGatherer )
add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx mpiCode/${FILE_NAME}_mpi.cxx
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.* .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/GujaratAgent.hxx .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py ${NAMESPACE} ${FILE_NAME}
    DEPENDS main.cxx ${FILE_NAME}.cxx ${FILE_NAME}.hxx
)

find_package( OpenMP )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )

# add the executable
add_executable( ${EX_NAME}
                AgentController.cxx
                Climate.cxx
                DoNothingAction.cxx
                ForageAction.cxx
                GujaratAgent.cxx
                GujaratConfig.cxx
                GujaratState.cxx
                GujaratWorld.cxx
                HunterGatherer.cxx
                HunterGathererDecisionTreeController.cxx
                HunterGathererMDPConfig.cxx
                HunterGathererMDPController.cxx
                HunterGathererMDPModel.cxx
                HunterGathererMDPState.cxx
                HunterGathererProgrammedController.cxx
                main.cxx
                MDPAction.cxx
                MoveHomeAction.cxx
                OriginalDemographics.cxx
                Sector.cxx
                SettlementAreas.cxx
                mpiCode/FactoryCode.cxx
                mpiCode/${FILE_NAME}_mpi.cxx
                )
target_link_libraries( ${EX_NAME} pandora-shared )

add_subdirectory( analysis )

install( TARGETS    ${EX_NAME}      DESTINATION examples/${EX_NAME} )
install( FILES      config.xml      DESTINATION examples/${EX_NAME} )
install( DIRECTORY  resources doc   DESTINATION examples/${EX_NAME} )
