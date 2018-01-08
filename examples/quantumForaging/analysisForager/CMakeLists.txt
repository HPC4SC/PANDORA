set( EX_NAME analysisForager )

# add the executable
add_executable( ${EX_NAME}
                main.cxx
                )
target_link_libraries( ${EX_NAME}
                       pandora-shared
                     )
install( TARGETS ${EX_NAME}
         DESTINATION examples/quantumForaging/${EX_NAME} )
