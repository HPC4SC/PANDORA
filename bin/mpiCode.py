#!/usr/bin/env python
import generateMpi
import os
import sys

if (len(sys.argv) == 1) :
    ns  = None
    fns = []
else :
    ns = sys.argv[1]
    fns = sys.argv[2:]

not os.path.exists('mpiCode') and os.mkdir('mpiCode')

target = [ 'mpiCode/FactoryCode.cxx' ]
source = [ 'main.cxx' ]
nspace = []
for fn in fns :
    target.append( 'mpiCode/'+fn+'_mpi.cxx' )
    source.append( fn+'.cxx' )
    nspace.append( ns )
env = {'namespaces': nspace }

generateMpi.execute( target, source, env )
