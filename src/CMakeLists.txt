set( PANDORA_FILES
     Action.cxx
     Agent.cxx
     AgentRecord.cxx
     Config.cxx
     DynamicRaster.cxx
     GeneralState.cxx
     IncrementalRaster.cxx
     LoggerBase.cxx
     MpiFactory.cxx
     OpenMPSingleNode.cxx
     RasterLoader.cxx
     SequentialSerializer.cxx
     Serializer.cxx
     ShpLoader.cxx
     SimulationRecord.cxx
     SpacePartition.cxx
     StaticRaster.cxx
     Statistics.cxx
     World.cxx
     analysis/AgentHDFtoSHP.cxx
     analysis/AgentHistogram.cxx
     analysis/AgentMean.cxx
     analysis/AgentNum.cxx
     analysis/AgentStdDev.cxx
     analysis/AgentSum.cxx
     analysis/Analysis.cxx
     analysis/GlobalAgentStats.cxx
     analysis/GlobalRasterStats.cxx
     analysis/IndividualStats.cxx
     analysis/Output.cxx
     analysis/RasterMean.cxx
     analysis/RasterSum.cxx )

add_library( pandora-shared SHARED ${PANDORA_FILES} )
add_library( pandora-static STATIC ${PANDORA_FILES} )

set_target_properties( pandora-shared PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "pandora" )
set_target_properties( pandora-static PROPERTIES CLEAN_DIRECT_OUTPUT 1 OUTPUT_NAME "pandora" )

target_link_libraries( pandora-shared
                       ${HDF5_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${GDAL_LIBRARY}
                       ${MPI_CXX_LIBRARIES}
                       ${PYTHON_LIBRARIES}
                       tinyxml-shared
                     )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

install( TARGETS pandora-shared pandora-static
         DESTINATION lib )

install( DIRECTORY ./ DESTINATION include
         FILES_MATCHING PATTERN "*.hxx"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )
