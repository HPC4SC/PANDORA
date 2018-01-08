set( PYPANDORA_FILES pyBindings.cxx )

add_library( pyPandora-shared SHARED ${PYPANDORA_FILES} )
set_target_properties( pyPandora-shared PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "pyPandora")

target_link_libraries( pyPandora-shared
                       pandora-shared
                       pandora_utils-shared
                       ${PYTHON_LIBRARIES}
                     )
#                       ${Boost_LIBRARIES}

install( TARGETS pyPandora-shared DESTINATION lib )

