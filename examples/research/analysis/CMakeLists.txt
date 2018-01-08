set( EX_NAME analysisResearch )

# add the executable
add_executable( ${EX_NAME} main.cxx )

target_link_libraries( ${EX_NAME} pandora-shared pandora_utils-shared )

install( TARGETS ${EX_NAME} DESTINATION examples/research/analysis )
