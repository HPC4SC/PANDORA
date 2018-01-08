set( EX_NAME musicalChairs )
install( FILES      config.xml README   DESTINATION examples/${EX_NAME} )
install( PROGRAMS   oasis.py            DESTINATION examples/${EX_NAME} )
install( DIRECTORY  analysis            DESTINATION examples/${EX_NAME} )
