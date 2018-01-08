set( EX_NAME research )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( NAMESPACE Examples )
set( FILE_NAME  Author )
set( FILE_NAME2 Paper )
add_custom_command(
    OUTPUT   mpiCode/FactoryCode.cxx mpiCode/${FILE_NAME}_mpi.cxx mpiCode/${FILE_NAME2}_mpi.cxx 
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.* .
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME2}.* .
    COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/bin ${CMAKE_SOURCE_DIR}/bin/mpiCode.py ${NAMESPACE} ${FILE_NAME} ${FILE_NAME2}
    DEPENDS main.cxx ${FILE_NAME}.cxx ${FILE_NAME}.hxx ${FILE_NAME2}.cxx ${FILE_NAME2}.hxx 
)

# add the executable
add_executable( ${EX_NAME}
                AcademiaConfig.cxx
                Academia.cxx
                Author.cxx
                main.cxx
                Paper.cxx
                mpiCode/FactoryCode.cxx
                mpiCode/${FILE_NAME}_mpi.cxx
                mpiCode/${FILE_NAME2}_mpi.cxx
                )
target_link_libraries( ${EX_NAME} pandora-shared )

add_subdirectory( analysis )

install( TARGETS    ${EX_NAME}          DESTINATION examples/${EX_NAME} )
install( FILES      config.xml README   DESTINATION examples/${EX_NAME} )
