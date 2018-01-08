set( UTILS_FILES MpiStubCode.cxx )

add_library( pandora_utils-shared SHARED ${UTILS_FILES} )
add_library( pandora_utils-static STATIC ${UTILS_FILES} )

set_target_properties( pandora_utils-shared PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "pandora_utils")
set_target_properties( pandora_utils-static PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "pandora_utils")

install( TARGETS pandora_utils-shared pandora_utils-static DESTINATION lib )
