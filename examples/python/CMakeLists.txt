set( EX_NAME python )

set( CSVFILES agents.csv resources.csv )
set( PYFILES testAgent.py testAnalysis.py testPoint.py testRaster.py testWorld.py )
install( FILES      ${CSVFILES} DESTINATION examples/${EX_NAME} )
install( PROGRAMS   ${PYFILES}  DESTINATION examples/${EX_NAME} )
