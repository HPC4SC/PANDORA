set( TINYXML_FILES 
    tinyxmlerror.cpp
    tinyxmlparser.cpp
    tinystr.cpp
    tinyxml.cpp )


add_library( tinyxml-shared SHARED ${TINYXML_FILES} )
add_library( tinyxml-static STATIC ${TINYXML_FILES} )

set_target_properties( tinyxml-shared PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "tinyxml")
set_target_properties( tinyxml-static PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "tinyxml")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

install( TARGETS tinyxml-shared tinyxml-static
         DESTINATION lib )

install( DIRECTORY ./ DESTINATION include
         FILES_MATCHING PATTERN "*.h"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
