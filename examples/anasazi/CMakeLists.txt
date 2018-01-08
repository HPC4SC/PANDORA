set( EX_NAME anasazi )

install( FILES      config.xml README   DESTINATION examples/${EX_NAME} )
install( PROGRAMS   virtualAnasazi.py   DESTINATION examples/${EX_NAME} )
install( DIRECTORY  resources           DESTINATION examples/${EX_NAME} )
