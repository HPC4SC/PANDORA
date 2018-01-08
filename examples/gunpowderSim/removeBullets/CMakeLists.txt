set( EX_NAME removeBullets )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py
    DEPENDS main.cxx
)

# add the executable
add_executable( ${EX_NAME}
                main.cxx
                RemoveBulletConfig.cxx
                RemoveBulletWorld.cxx
                mpiCode/FactoryCode.cxx )
target_link_libraries( ${EX_NAME} pandora-shared )

install( TARGETS    ${EX_NAME} DESTINATION  examples/gunpowderSim/${EX_NAME} )
install( FILES      config.xml DESTINATION  examples/gunpowderSim/${EX_NAME} )
