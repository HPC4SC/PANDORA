#!/usr/bin/env python3
import os
import collections

def writeRegisterTypes(f, listAgents):
    f.write('void MpiFactory::registerTypes()\n')
    f.write('{\n')
    for i, agent in enumerate(listAgents):
        f.write('\t_types.insert( std::make_pair( "' + agent + '", create' + agent + 'Type()));\n')
        f.write('\t_typesMapNameToID.insert( std::make_pair( "' + agent + '", ' + str(i) + '));\n')
        f.write('\t_typesMapIDToName.insert( std::make_pair( ' + str(i) + ', "' + agent + '"));\n')
    f.write('}\n')
    f.write('\n')


def writeCreateDefaultPackage(f, listAgents):
    f.write('void * MpiFactory::createDefaultPackage( const std::string & type )\n')
    f.write('{\n')
    for i, agent in enumerate(listAgents):
        f.write('\tif(type.compare("' + agent + '")==0)\n')
        f.write('\t{\n')
        f.write('\t\treturn new ' + agent + 'Package;\n')
        f.write('\t}\n')
    f.write('\n')
    f.write('\tstd::stringstream oss;\n')
    f.write('\toss << "MpiFactory::createDefaultPackage - unknown agent type: " << type;\n')
    f.write('\tthrow Engine::Exception(oss.str());\n')
    f.write('\treturn 0;\n')
    f.write('}\n')
    f.write('\n')
    return None


def writeCreateAndFillAgents(f, listAgents, namespaces):
    f.write('Agent * MpiFactory::createAndFillAgent( const std::string & type, void * package )\n')
    f.write('{\n')
    for i, agent in enumerate(listAgents):
        f.write('\t')
        if (i > 0): f.write('else ')
        f.write('if(type.compare("' + agent + '")==0)\n')
        f.write('\t{\n')
        f.write('\t\treturn new ' + namespaces[i] + "::" + agent + '(package);\n')
        f.write('\t}\n')
    f.write('\n')
    f.write('\tstd::stringstream oss;\n')
    f.write('\toss << "MpiFactory::createDefaultPackage - unknown agent type: " << type;\n')
    f.write('\tthrow Engine::Exception(oss.str());\n')
    f.write('\treturn 0;\n')

    f.write('}\n')
    f.write('\n')
    return None

def writeGetSizeOfPackage(f, listAgents):
    f.write('int MpiFactory::getSizeOfPackage(const std::string& type) const\n')
    f.write('{\n')
    for i, agent in enumerate(listAgents):
        f.write('\t')
        if (i > 0): f.write('else ')
        f.write('if(type.compare("' + agent + '") == 0)\n')
        f.write('\t{\n')
        f.write('\t\t' + agent + 'Package* ' + agent.lower() + 'Package = new ' + agent + 'Package;\n')
        f.write('\t\tint resultingSize = sizeof(*' + agent.lower() + 'Package);\n')
        f.write('\t\tdelete ' + agent.lower() + 'Package;\n')
        f.write('\t\treturn resultingSize;\n')
        f.write('\t}\n')
    f.write('\n')
    f.write('\tstd::stringstream oss;\n')
    f.write('\toss << "MpiFactory::getSizeOfPackage - unknown agent type: " << type;\n')
    f.write('\tthrow Engine::Exception(oss.str());\n')

    f.write('}\n')
    f.write('\n')
    return None

def writeFreePackage(f, listAgents):
    f.write('void MpiFactory::freePackage(void* package, const std::string& type) const\n')
    f.write('{\n')
    for i, agent in enumerate(listAgents):
        f.write('\t')
        if (i > 0): f.write('else ')
        f.write('if(type.compare("' + agent + '") == 0)\n')
        f.write('\t{\n')
        f.write('\t\t' + agent + 'Package* ' + agent.lower() + 'Package = static_cast<' + agent + 'Package*>(package);\n')
        f.write('\t\tdelete ' + agent.lower() + 'Package;\n')
        f.write('\t\treturn;\n')
        f.write('\t}\n')
    f.write('\n')
    f.write('\tstd::stringstream oss;\n')
    f.write('\toss << "MpiFactory::freePackage - unknown agent type: " << type;\n')
    f.write('\tthrow Engine::Exception(oss.str());\n')

    f.write('}\n')
    f.write('\n')
    return None

def getMpiTypeAttribute(typeAttribute):
    mpiTypeAttribute = 'MPI_INT'
    if (typeAttribute == 'float'):
        mpiTypeAttribute = 'MPI_FLOAT'
    return mpiTypeAttribute


def writeCreateType(f, nameAgent, attributesMap):
    # we have to send 6 basic attributes (_id, _position._x, _position._y, _discretePosition._x, _discretePosition._y, _layer, _discreteLayer & _exists + the number of dynamic attributes
    numBasicAttributes = 8
    numAttributes = numBasicAttributes + len(attributesMap)
    f.write('MPI_Datatype * create' + nameAgent + 'Type()\n')
    f.write('{\n')
    f.write('\t' + nameAgent + 'Package package;\n')
    f.write('\n')
    # lengths and types
    f.write('\tint blockLengths[' + str(numAttributes) + '];\n')
    f.write('\tMPI_Datatype typeList[' + str(numAttributes) + '];\n')
    f.write('\t// block lengths and types\n')
    # id, position & exists
    f.write('\t// _id\n')
    f.write('\tblockLengths[0] = 32;\n');
    f.write('\ttypeList[0] = MPI_CHAR;\n');
    f.write('\t// _position._x\n')
    f.write('\tblockLengths[1] = 1;\n')
    f.write('\ttypeList[1] = MPI_INT;\n')
    f.write('\t// _position._y\n')
    f.write('\tblockLengths[2] = 1;\n')
    f.write('\ttypeList[2] = MPI_INT;\n')
    f.write('\t// _discretePosition._x\n')
    f.write('\tblockLengths[3] = 1;\n')
    f.write('\ttypeList[3] = MPI_INT;\n')
    f.write('\t// _discretePosition._y\n')
    f.write('\tblockLengths[4] = 1;\n')
    f.write('\ttypeList[4] = MPI_INT;\n')
    f.write('\t// _layer\n')
    f.write('\tblockLengths[5] = 1;\n')
    f.write('\ttypeList[5] = MPI_INT;\n')
    f.write('\t// _discreteLayer\n')
    f.write('\tblockLengths[6] = 1;\n')
    f.write('\ttypeList[6] = MPI_INT;\n')
    f.write('\t// _exists\n')
    f.write('\tblockLengths[7] = 1;\n')
    f.write('\ttypeList[7] = MPI_INT;\n')

    # dynamic params
    index = numBasicAttributes
    for nameAttribute, typeAttribute in attributesMap.items():
        f.write('\t// ' + nameAttribute + '\n')
        if typeAttribute == "string":
            f.write('\tblockLengths[' + str(index) + '] = 32;\n')
            f.write('\ttypeList[' + str(index) + '] = MPI_CHAR;\n');
        else:
            f.write('\tblockLengths[' + str(index) + '] = 1;\n')
            mpiTypeAttribute = getMpiTypeAttribute(typeAttribute)
            f.write('\ttypeList[' + str(index) + '] = ' + mpiTypeAttribute + ';\n')
        index = index + 1
    f.write('\n')
    # displacements
    f.write('\tMPI_Aint displacements[' + str(numAttributes) + '];\n')
    f.write('\tMPI_Aint startAddress;\n')
    f.write('\tMPI_Aint address;\n')

    # id, position, discretePosition & exists
    f.write('\t// id\n')
    f.write('\tMPI_Address(package._idMpi, &startAddress);\n')
    f.write('\tdisplacements[0] = 0;\n')

    f.write('\t// _position._x\n')
    f.write('\tMPI_Address(&package._positionMpi._x, &address);\n')
    f.write('\tdisplacements[1] = address-startAddress;\n')

    f.write('\t// _position._y\n')
    f.write('\tMPI_Address(&package._positionMpi._y, &address);\n')
    f.write('\tdisplacements[2] = address-startAddress;\n')

    f.write('\t// _discretePosition._x\n')
    f.write('\tMPI_Address(&package._discretePositionMpi._x, &address);\n')
    f.write('\tdisplacements[3] = address-startAddress;\n')

    f.write('\t// _discretePosition._y\n')
    f.write('\tMPI_Address(&package._discretePositionMpi._y, &address);\n')
    f.write('\tdisplacements[4] = address-startAddress;\n')

    f.write('\t// _layer\n')
    f.write('\tMPI_Address(&package._layerMpi, &address);\n')
    f.write('\tdisplacements[5] = address-startAddress;\n')

    f.write('\t// _discreteLayer\n')
    f.write('\tMPI_Address(&package._discreteLayerMpi, &address);\n')
    f.write('\tdisplacements[6] = address-startAddress;\n')

    f.write('\t// _exists\n')
    f.write('\tMPI_Address(&package._existsMpi, &address);\n')
    f.write('\tdisplacements[7] = address-startAddress;\n')

    # dynamic params
    index = numBasicAttributes
    for nameAttribute, typeAttribute in attributesMap.items():
        f.write('\t// ' + nameAttribute + '\n')
        f.write('\tMPI_Address(&package.' + nameAttribute + 'Mpi, &address);\n')
        f.write('\tdisplacements[' + str(index) + '] = address-startAddress;\n')
        index = index + 1

    # create mpi data type  
    f.write('\n')
    f.write('\tMPI_Datatype * newDataType = new MPI_Datatype;\n')
    f.write('\tMPI_Type_struct(' + str(numAttributes) + ', blockLengths, displacements, typeList, newDataType);\n')
    f.write('\tMPI_Type_commit(newDataType);\n')
    f.write('\treturn newDataType;\n')

    f.write('}\n')
    f.write('\n')
    return None


def createFactoryMethods(listAgents, factoryFile, namespaces, listAttributesMaps):
    f = open(factoryFile, 'w')
    print '\tcreating mpi factory: ' + factoryFile
    # headers
    f.write('\n')
    f.write('#include <MpiFactory.hxx>\n')
    f.write('#include <Exception.hxx>\n')
    f.write('#include <sstream>\n')
    f.write('\n')
    for i, agent in enumerate(listAgents):
        print '\t\tadding: ' + agent + ' to factory file: ' + factoryFile
        f.write('#include <' + agent + '.hxx>\n')
        f.write('#include "' + agent + '_mpi.hxx"\n')
    f.write('\n')
    f.write('namespace Engine\n')
    f.write('{\n')
    f.write('\n')

    for i, agent in enumerate(listAgents):
        writeCreateType(f, agent, listAttributesMaps[i])

    writeRegisterTypes(f, listAgents)
    writeCreateDefaultPackage(f, listAgents)
    writeCreateAndFillAgents(f, listAgents, namespaces)
    writeGetSizeOfPackage(f, listAgents)
    writeFreePackage(f, listAgents)

    # close header & namespace
    f.write('} // namespace Engine\n')
    f.write('\n')
    f.close()
    return None


def createMpiHeader(agentName, source, header, attributesMap):
    print '\t\tcreating mpi header: mpiCode/' + agentName + '_mpi.hxx for agent: ' + agentName + ' from source: ' + source + ' and header: ' + header
    f = open('mpiCode/' + agentName + '_mpi.hxx', 'w')
    # header
    f.write('\n')
    f.write('#ifndef __' + agentName + '_mpi_hxx\n')
    f.write('#define __' + agentName + '_mpi_hxx\n')
    f.write('\n')
    f.write('#include <Point2D.hxx>\n')
    f.write('#include <' + agentName + '.hxx>\n')
    f.write('\n')
    # struct Package
    f.write('typedef struct\n')
    f.write('{\n')
    # basic data: _id, _position, _discretePosition & _exists
    f.write('\tchar _idMpi[32];\n')
    f.write('\tbool _existsMpi;\n')
    f.write('\tEngine::Point2D<int> _positionMpi;\n')
    f.write('\tEngine::Point2D<int> _discretePositionMpi;\n')
    f.write('\tint _layerMpi;\n')
    f.write('\tint _discreteLayerMpi;\n')
    f.write('\n')
    # dynamic params
    for nameAttribute, typeAttribute in attributesMap.items():
        if typeAttribute == "string":
            f.write('\tchar ' + nameAttribute + 'Mpi[32];\n')
        else:
            f.write('\t' + typeAttribute + ' ' + nameAttribute + 'Mpi;\n')
    f.write('\n')
    # close struct
    f.write('} ' + agentName + 'Package;\n')
    # close file
    f.write('\n')
    f.write('#endif // __' + agentName + '_mpi_hxx\n')
    f.write('\n')
    f.close()
    return None


def writeFillPackage(f, agentName, attributesMap):
    f.write('void * ' + agentName + '::fillPackage() const\n')
    f.write('{\n')
    # basic params: _id, _position, _discretePosition & _exists
    f.write('\t' + agentName + 'Package * package = new ' + agentName + 'Package;\n')
    f.write(
        '\tmemcpy(&package->_idMpi, _id.c_str(), std::min((unsigned int)32,(unsigned int)(sizeof(char)*_id.size())));\n')
    f.write('\tpackage->_idMpi[std::min((unsigned int)32,(unsigned int)_id.size())] = \'\\0\';\n')
    f.write('\tpackage->_existsMpi = _exists;\n')
    f.write('\tpackage->_positionMpi = _position;\n')
    f.write('\tpackage->_discretePositionMpi = _discretePosition;\n')
    f.write('\tpackage->_layerMpi = _layer;\n')
    f.write('\tpackage->_discreteLayerMpi = _discreteLayer;\n')
    f.write('\n')
    # dynamic params
    for nameAttribute, typeAttribute in attributesMap.items():
        if typeAttribute == "string":
            f.write(
                '\tmemcpy(&package->' + nameAttribute + 'Mpi, ' + nameAttribute + '.c_str(), std::min((unsigned int)32,(unsigned int)(sizeof(char)*' + nameAttribute + '.size())));\n');
            f.write(
                '\tpackage->' + nameAttribute + 'Mpi[std::min((unsigned int)32,(unsigned int)' + nameAttribute + '.size())] = \'\\0\';\n')
        else:
            f.write('\tpackage->' + nameAttribute + 'Mpi = ' + nameAttribute + ';\n')
    f.write('\treturn package;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writeFreeAgentPackage(f, agentName):
    f.write('void ' + agentName + '::freePackage(void* package) const\n')
    f.write('{\n')
    f.write('\t' + agentName + 'Package* ' + agentName.lower() + 'Package = static_cast<' + agentName + 'Package*>(package);\n')
    f.write('\tdelete ' + agentName.lower() + 'Package;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writeConstructor(f, agentName, parent, attributesMap):
    f.write(
        agentName + '::' + agentName + '( void * package ) : ' + parent + '(((' + agentName + 'Package*)package)->_idMpi)\n')
    f.write('{\n')
    f.write('\t' + agentName + 'Package * particularPackage = (' + agentName + 'Package*)package;\n')
    # basic params: _position & _exists
    f.write('\t_exists = particularPackage->_existsMpi;\n')
    f.write('\t_position = particularPackage->_positionMpi;\n')
    f.write('\t_discretePosition = particularPackage->_discretePositionMpi;\n')
    f.write('\t_layer = particularPackage->_layerMpi;\n')
    f.write('\t_discreteLayer = particularPackage->_discreteLayerMpi;\n')
    f.write('\n')
    # dynamic params
    for nameAttribute in attributesMap.keys():
        f.write('\t' + nameAttribute + ' = particularPackage->' + nameAttribute + 'Mpi;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writeComparator(f, agentName, parent, attributesMap):
    f.write('bool ' + agentName + '::hasTheSameAttributes(const ' + parent + '& other) const\n')
    f.write('{\n')
    f.write('\tif (not ' + parent + '::hasTheSameAttributes(other)) return false;\n')
    f.write('\n')
    f.write('\tconst ' + agentName + '& otherCastedToBug = static_cast<const ' + agentName + '&>(other);\n')
    f.write('\n')
    f.write('\treturn ')
    firstAttribute = True
    for nameAttribute, typeAttribute in attributesMap.items():
        if not firstAttribute:
            f.write(' and\n\t\t')
        f.write('\t' + nameAttribute + ' == otherCastedToBug.' + nameAttribute)
        firstAttribute = False
    f.write(';\n')
    f.write('}\n')
    return None

def getMpiTypeConversion(typeInCpp):
    if typeInCpp == 'int':
        return 'MPI_INTEGER'
    elif typeInCpp == 'float':
        return 'MPI_FLOAT'

    return 'MPI_INTEGER'


def writeVectorAttributesPassing(f, agentName, vectorAttributesMap):
    print 'writing vector attributes map'
    f.write('\n')
    f.write('void ' + agentName + '::sendVectorAttributes( int target )\n')
    f.write('{\n')
    if vectorAttributesMap:
        f.write('\tint sizeVector = 0;\n')
    for nameAttribute in vectorAttributesMap.keys():
        print 'sending vector: ' + nameAttribute + ' with type: ' + vectorAttributesMap[nameAttribute]
        f.write('\tsizeVector = ' + nameAttribute + '.size();\n')
        f.write('\tMPI_Send(&sizeVector, 1, MPI_INTEGER, target, Engine::eSizeVector, MPI_COMM_WORLD);\n')
        mpiType = getMpiTypeConversion(vectorAttributesMap[nameAttribute])
        f.write(
            '\tMPI_Send(&' + nameAttribute + '[0], sizeVector, ' + mpiType + ', target, Engine::eVectorAttribute, MPI_COMM_WORLD);\n')
        f.write('\n')
    f.write('}\n')
    f.write('\n')

    f.write('void ' + agentName + '::receiveVectorAttributes( int origin )\n')
    f.write('{\n')
    if vectorAttributesMap:
        f.write('\tint sizeVector = 0;\n')
        f.write('\tMPI_Status status;\n')
    for nameAttribute in vectorAttributesMap.keys():
        print 'receiving vector: ' + nameAttribute + ' with type: ' + vectorAttributesMap[nameAttribute]
        f.write('\tMPI_Recv(&sizeVector, 1, MPI_INTEGER, origin, Engine::eSizeVector, MPI_COMM_WORLD, &status);\n')
        f.write('\t' + nameAttribute + '.resize(sizeVector);\n')
        mpiType = getMpiTypeConversion(vectorAttributesMap[nameAttribute])
        f.write(
            '\tMPI_Recv(&' + nameAttribute + '[0], sizeVector, ' + mpiType + ', origin, Engine::eVectorAttribute, MPI_COMM_WORLD, &status);\n')
        f.write('\n')
    f.write('}\n')
    f.write('\n')
    return None

def getMethodsForVector(agentName, variableID, complexAttributesRelated, deltaHeadersAndCodes):
    variableName = complexAttributesRelated.complexAttributesOrderMap[variableID]
    typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]
    headerWithParams = complexAttributesRelated.mapOfDeltaHeaders[variableID]
    deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

    insertMethodCode =  '\tif (index >= ' + variableName + '.size() or index < 0)\n' \
                        '\t\t' + variableName + '.push_back(element);\n' \
                        '\telse\n' \
                        '\t{\n' \
                        '\t\tstd::vector<' + typeOfElements + '>::const_iterator itPos = ' + variableName + '.begin() + index;\n' \
                        '\t\t' + variableName + '.insert(itPos, element);\n' \
                        '\t}\n' \
                        '\n' \
                        '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eVectorInsert, index, element));\n'
    updateMethodCode =  '\tif (index >= ' + variableName + '.size() or index < 0) throw Engine::Exception("' + agentName + '_mpi.cxx::updateElementAtIndex_forVector' + variableName + '() - NOT VALID INDEX.");\n' \
                        '\n' \
                        '\t' + variableName + '.at(index) = element;\n' \
                        '\n' \
                        '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eVectorUpdate, index, element));\n'

    typeOfElementsNull = '(' + typeOfElements + ') 0'
    if typeOfElements.find('Engine::Point2D') != -1:
        typeOfElementsNull = 'Engine::Point2D<int>()'

    deleteMethodCode =  '\tif (index >= ' + variableName + '.size() or index < 0) throw Engine::Exception("' + agentName + '_mpi.cxx::deleteElementAtIndex_forVector' + variableName + '() - NOT VALID INDEX.");\n' \
                        '\n' \
                        '\t' + variableName + '.erase(' + variableName + '.begin() + index);\n' \
                        '\n' \
                        '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eVectorDelete, index, ' + typeOfElementsNull + '));\n'

    deltaHeadersAndCodes[headerWithParams['insert']] = insertMethodCode
    deltaHeadersAndCodes[headerWithParams['update']] = updateMethodCode
    deltaHeadersAndCodes[headerWithParams['delete']] = deleteMethodCode

    return None

def getMethodsForQueue(variableID, complexAttributesRelated, deltaHeadersAndCodes):
    variableName = complexAttributesRelated.complexAttributesOrderMap[variableID]
    typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]
    headerWithParams = complexAttributesRelated.mapOfDeltaHeaders[variableID]
    deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

    pushMethodCode =    '\t' + variableName + '.push(element);\n' \
                        '\n' \
                        '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eQueuePush, element));\n'
    
    typeOfElementsNull = '(' + typeOfElements + ') 0'
    if typeOfElements.find('Engine::Point2D') != -1:
        typeOfElementsNull = 'Engine::Point2D<int>()'

    popMethodCode =     '\tif (not ' + variableName + '.empty())\n' \
                        '\t{\n' \
                        '\t\t' + variableName + '.pop();\n' \
                        '\n' \
                        '\t\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eQueuePop, ' + typeOfElementsNull + '));\n' \
                        '\t}\n'

    deltaHeadersAndCodes[headerWithParams['push']] = pushMethodCode
    deltaHeadersAndCodes[headerWithParams['pop']] = popMethodCode

    return None

def getMethodsForMap(variableID, complexAttributesRelated, deltaHeadersAndCodes):
    variableName = complexAttributesRelated.complexAttributesOrderMap[variableID]
    typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]
    valueType = complexAttributesRelated.complexAttributesValueInMapType[variableID]
    headerWithParams = complexAttributesRelated.mapOfDeltaHeaders[variableID]
    deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

    insertOrUpdateMethodCode =  '\t' + variableName + '[key] = value;\n' \
                                '\n' \
                                '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eMapInsertOrUpdate, key, value));\n'

    valueTypeNull = '(' + valueType + ') 0'
    if valueType.find('Engine::Point2D') != -1:
        valueTypeNull = 'Engine::Point2D<int>()'

    deleteMethodCode =          '\t' + variableName + '.erase(key);\n' \
                                '\n' \
                                '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eMapDelete, key, ' + valueTypeNull + '));\n'

    deltaHeadersAndCodes[headerWithParams['insertOrUpdate']] = insertOrUpdateMethodCode
    deltaHeadersAndCodes[headerWithParams['delete']] = deleteMethodCode

    return None

def getListOfSetterHeaders(agentName, complexAttributesRelated):
    complexAttributesOrderMap = complexAttributesRelated.complexAttributesOrderMap

    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        print 'variableName "' + variableName + '" of type "' + complexAttributesRelated.complexAttributesFullType[variableID] + '"'

        deltaVariableName = '_deltaFor' + variableName
        complexAttributesRelated.mapOfDeltaVariablesName[variableID] = deltaVariableName

        typeOfElements = ''
        if complexAttributesRelated.complexAttributesFullType[variableID].find('<Engine::Point2D') != -1:
            typeOfElements = 'Engine::Point2D<int>'
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('<int') != -1:
            typeOfElements = 'int'
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('<float') != -1:
            typeOfElements = 'float'
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('<bool') != -1:
            typeOfElements = 'bool'
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('<std::string') != -1:
            typeOfElements = 'std::string'
        else:
            print 'No valid type for attribute "' + variableName + '" of type "' + complexAttributesRelated.complexAttributesFullType[variableID] + '"!'

        complexAttributesRelated.complexAttributesElementsType[variableID] = typeOfElements

        if complexAttributesRelated.complexAttributesFullType[variableID].find('std::vector') != -1:
            complexAttributesRelated.mapOfDeltaVariablesType[variableID] = 'std::vector<std::tuple<int, int, ' + typeOfElements + '>>'
            complexAttributesRelated.mapOfDeltaVariablesComments[variableID] = '// <typeOfModification, elementIndex, elementNewValue>'
            
            complexAttributesRelated.complexAttributesShortType[variableID] = 'std::vector'

            complexAttributesRelated.mapOfDeltaHeaders[variableID]['insert'] = 'insertElementAtIndex' + variableName + '(const ' + typeOfElements + '& element, const int& index)'
            complexAttributesRelated.mapOfDeltaHeaders[variableID]['update'] = 'updateElementAtIndex' + variableName + '(const ' + typeOfElements + '& element, const int& index)'
            complexAttributesRelated.mapOfDeltaHeaders[variableID]['delete'] = 'deleteElementAtIndex' + variableName + '(const int& index)'
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('std::queue') != -1:
            complexAttributesRelated.mapOfDeltaVariablesType[variableID] = 'std::vector<std::tuple<int, ' + typeOfElements + '>>'
            complexAttributesRelated.mapOfDeltaVariablesComments[variableID] = '// <typeOfModification, elementNewValue>'

            complexAttributesRelated.complexAttributesShortType[variableID] = 'std::queue'

            complexAttributesRelated.mapOfDeltaHeaders[variableID]['push'] = 'pushElement' + variableName + '(const ' + typeOfElements + '& element)'
            complexAttributesRelated.mapOfDeltaHeaders[variableID]['pop'] = 'popElement' + variableName + '()'
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('std::map') != -1:
            valueType = ''
            if complexAttributesRelated.complexAttributesFullType[variableID].find(',Engine::Point2D<int>') != -1:
                valueType = 'Engine::Point2D<int>'
            elif complexAttributesRelated.complexAttributesFullType[variableID].find(',int') != -1:
                valueType = 'int'
            elif complexAttributesRelated.complexAttributesFullType[variableID].find(',float') != -1:
                valueType = 'float'
            elif complexAttributesRelated.complexAttributesFullType[variableID].find(',bool') != -1:
                valueType = 'bool'
            elif complexAttributesRelated.complexAttributesFullType[variableID].find(',std::string') != -1:
                valueType = 'std::string'
            else:
                print 'No valid value type for map "' + variableName + '" of type "' + complexAttributesRelated.complexAttributesFullType[variableID] + '"!'

            complexAttributesRelated.mapOfDeltaVariablesType[variableID] = 'std::vector<std::tuple<int, ' + typeOfElements + ', ' + valueType + '>>'
            complexAttributesRelated.mapOfDeltaVariablesComments[variableID] = '// <typeOfModification, elementKey, elementValue>'

            complexAttributesRelated.complexAttributesShortType[variableID] = 'std::map'
            complexAttributesRelated.complexAttributesValueInMapType[variableID] = valueType

            complexAttributesRelated.mapOfDeltaHeaders[variableID]['insertOrUpdate'] = 'insertOrUpdateElement' + variableName + '(const ' + typeOfElements + '& key, const ' + valueType + '& value)'
            complexAttributesRelated.mapOfDeltaHeaders[variableID]['delete'] = 'deleteElement' + variableName + '(const ' + typeOfElements + '& key)'

    return None

def writeComplexAttributesSettersHeaders(agentName, headerName, complexAttributesRelated):
    # Building headers
    firstAttributeMethod = complexAttributesRelated.complexAttributesOrderMap[1] + '(const '
    f = open(headerName, 'r')
    for line in f:
        if line.find(firstAttributeMethod) != -1:
            print '\tComplex attributes setters headers at "' + headerName + '" already correct.'
            return
    f.close()

    print '\t"' + headerName + '" does not contain the complex attributes setters headers, adding them...'

    headerNameTmp = headerName + '_tmp'
    f = open(headerName, 'r')
    fTmp = open(headerNameTmp, 'w')

    insideClass = 0
    for line in f:
        if insideClass == 0:
            if line.find('class') != -1 and line.find(agentName) != -1:
                print 'accessing agent declaration: ' + agentName
                insideClass = 1
            fTmp.write(line)
        elif line.find('};') != -1:
            print 'end of agent declaration: ' + agentName
            insideClass = 0
            fTmp.write('\n')
            
            fTmp.write('\t///// Autogenerated code (do not modify): /////\n')
            fTmp.write('private:\n')
            for variableID in complexAttributesRelated.complexAttributesOrderMap:
                fTmp.write('\t' + complexAttributesRelated.mapOfDeltaVariablesType[variableID] + ' ' + complexAttributesRelated.mapOfDeltaVariablesName[variableID] + ';\t\t\t' + complexAttributesRelated.mapOfDeltaVariablesComments[variableID] + '\n')

            fTmp.write('\n')
            
            fTmp.write('public:\n')
            for variableID, classifiedMethods in complexAttributesRelated.mapOfDeltaHeaders.items():
                for methodType, methodWithParams in classifiedMethods.items():
                    fTmp.write('\tvoid ' + methodWithParams + ';\n')

            fTmp.write('\t///////// End of autogenerated code ///////////\n')
            fTmp.write('\n')
            fTmp.write(line)
        else:
            fTmp.write(line)
            
    f.close()
    fTmp.close()
    os.rename(headerNameTmp, headerName)

    return None

def writeComplexAttributesSettersCode(f, agentName, complexAttributesRelated):
    deltaHeadersAndCodes = {}
    for variableID in complexAttributesRelated.complexAttributesOrderMap:
        attributeType = complexAttributesRelated.complexAttributesShortType[variableID]

        if attributeType == 'std::vector':
            getMethodsForVector(agentName, variableID, complexAttributesRelated, deltaHeadersAndCodes)
        elif attributeType == 'std::queue':
            getMethodsForQueue(variableID, complexAttributesRelated, deltaHeadersAndCodes)
        elif attributeType == 'std::map':
            getMethodsForMap(variableID, complexAttributesRelated, deltaHeadersAndCodes)

    for header, code in deltaHeadersAndCodes.items():
        f.write('void ' + agentName + '::' + header + '\n')
        f.write('{\n')
        f.write(code)
        f.write('}\n')
        f.write('\n')

    return None

def writePoints2DVariablesDefinitionParametrized(f, typeOfElements, dsVariableName):
    if typeOfElements == 'Engine::Point2D<int>':
        f.write('\n')
        f.write('\t\tint ' + dsVariableName + 'X = ' + dsVariableName + '.getX();\n')
        f.write('\t\tint ' + dsVariableName + 'Y = ' + dsVariableName + '.getY();\n')

def writePoints2DMemCpyParametrized(f, typeOfElements, dsVariableName):
    if typeOfElements == 'Engine::Point2D<int>':
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + 'X, sizeof(int));\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + 'Y, sizeof(int));\t\tbyteIndex += sizeof(int);\n')
    else:
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + ', sizeof(' + typeOfElements + '));\t\t\tbyteIndex += sizeof(' + typeOfElements + ');\n')

def writeCreateComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated):
    f.write('void* ' + agentName + '::createComplexAttributesDeltaPackage(int& sizeOfPackage)\n')
    f.write('{\n')

    f.write('\tstd::cout << CreateStringStream("Bug " << getId() << " in createComplexAttributesDeltaPackage\\n").str();\n')

    if len(complexAttributesRelated.complexAttributesOrderMap) == 0:
        f.write('}\n')
        f.write('\n')
        return None

    deltaSizeVariables = {}
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

        deltaSizeVariable = 'delta' + variableName + 'Size'
        f.write('\tint ' + deltaSizeVariable + ' = ' + deltaVariableName + '.size();\n')

        deltaSizeVariables[variableID] = deltaSizeVariable

    f.write('\tsizeOfPackage =\t')
    first = True
    for variableID, variableShortType in complexAttributesRelated.complexAttributesShortType.items():
        if not first:
            f.write('\t\t\t\t\t')

        f.write('\t(' + deltaSizeVariables[variableID] + ' > 0) * sizeof(int) + ')

        elementType = complexAttributesRelated.complexAttributesElementsType[variableID]
        sizeOfElementString = 'sizeof(' + elementType + ')'
        if elementType == 'Engine::Point2D<int>':
            sizeOfElementString = '2 * sizeof(int)'

        if variableShortType == 'std::vector':
            f.write(deltaSizeVariables[variableID] + ' * sizeof(int) + ' + deltaSizeVariables[variableID] + ' * sizeof(int) + ' + deltaSizeVariables[variableID] + ' * ' + sizeOfElementString)
        elif variableShortType == 'std::queue':
            f.write(deltaSizeVariables[variableID] + ' * sizeof(int) + ' + deltaSizeVariables[variableID] + ' * ' + sizeOfElementString)
        elif variableShortType == 'std::map':
            valueInMapType = complexAttributesRelated.complexAttributesValueInMapType[variableID]
            sizeOfValueString = 'sizeof(' + valueInMapType + ')'
            if valueInMapType == 'Engine::Point2D<int>':
                sizeOfValueString = '2 * sizeof(int)'

            f.write(deltaSizeVariables[variableID] + ' * sizeof(int) + ' + deltaSizeVariables[variableID] + ' * ' + sizeOfElementString + ' + ' + deltaSizeVariables[variableID] + ' * ' + sizeOfValueString)

        if variableID != complexAttributesRelated.complexAttributesShortType.keys()[-1]:
            f.write(' +\n')
        else:
            f.write(';\n')

        first = False

    f.write('\n')
    f.write('\tvoid* resultingPackage = malloc(sizeOfPackage);\n')
    f.write('\tint byteIndex = 0;\n')
    f.write('\tint variableID;\n')
    f.write('\n')

    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]
        variableShortType = complexAttributesRelated.complexAttributesShortType[variableID]
        typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]

        f.write('\t// Delta elements for variable ' + variableName + ':\n')
        f.write('\t\tif (' + deltaSizeVariables[variableID] + ' > 0)\n')
        f.write('\t\t{\n')
        f.write('\t\t\tvariableID = ' + str(variableID) + ';\n')
        f.write('\t\t\tmemcpy((char*) resultingPackage + byteIndex, &variableID, sizeof(int));\t\t\t\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\t}\n')
        f.write('\tfor (int i = 0; i < ' + deltaSizeVariables[variableID] + '; ++i)\n')
        f.write('\t{\n')

        f.write('\t\tint typeOfModification = std::get<0>(' + deltaVariableName + '[i]);\n')
        if variableShortType == 'std::vector':
            f.write('\t\tint elementIndex = std::get<1>(' + deltaVariableName + '[i]);\n')
            f.write('\t\t' + typeOfElements + ' elementNewValue = std::get<2>(' + deltaVariableName + '[i]);\n')

            f.write('std::cout << CreateStringStream("hey1 - typeOfModification: " << typeOfModification << " elementIndex: " << elementIndex << " elementNewValue: " << elementNewValue << "\\n").str();\n')

        elif variableShortType == 'std::queue':
            f.write('\t\t' + typeOfElements + ' elementNewValue = std::get<1>(' + deltaVariableName + '[i]);\n')
        elif variableShortType == 'std::map':
            valueType = complexAttributesRelated.complexAttributesValueInMapType[variableID]

            f.write('\t\t' + typeOfElements + ' elementKey = std::get<1>(' + deltaVariableName + '[i]);\n')
            f.write('\t\t' + valueType + ' elementNewValue = std::get<2>(' + deltaVariableName + '[i]);\n')


        if variableShortType == 'std::map':
            writePoints2DVariablesDefinitionParametrized(f, typeOfElements, 'elementKey')
            writePoints2DVariablesDefinitionParametrized(f, valueType, 'elementNewValue')
        else:
            writePoints2DVariablesDefinitionParametrized(f, typeOfElements, 'elementNewValue')

        f.write('\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &typeOfModification, sizeof(int));\t\tbyteIndex += sizeof(int);\n')

        if variableShortType == 'std::map':
            writePoints2DMemCpyParametrized(f, typeOfElements, 'elementKey')
            writePoints2DMemCpyParametrized(f, valueType, 'elementNewValue')
        else:
            if variableShortType == 'std::vector':
                f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &elementIndex, sizeof(int));\t\t\tbyteIndex += sizeof(int);\n')
            writePoints2DMemCpyParametrized(f, typeOfElements, 'elementNewValue')

        f.write('\t}\n')
    f.write('\tstd::cout << CreateStringStream("Package " << getId() << " created in createComplexAttributesDeltaPackage: " << *((int*) (resultingPackage + 0)) << " where sizeOfPackage: " << sizeOfPackage << "\\n").str();\n')

    for variableID in complexAttributesRelated.complexAttributesOrderMap:
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]
        f.write('\t' + deltaVariableName + '.clear();\n')

    f.write('\treturn resultingPackage;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writeCopyContinuousValuesToDiscreteOnes(f, agentName, attributesMap, complexAttributesRelated):
    f.write('void ' + agentName + '::copyContinuousValuesToDiscreteOnes()\n')
    f.write('{\n')
    f.write('\tEngine::Agent::copyContinuousValuesToDiscreteOnes();\n')
    f.write('\n')
    for key in attributesMap:
        f.write('\t_discrete' + key + ' = ' + key + ';\n')
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        f.write('\t_discrete' + variableName + ' = ' + variableName + ';\n')
    f.write('}\n')
    f.write('\n')
    return None

def createMpiCode(agentName, source, headerName, namespace, parent, attributesMap, vectorAttributesMap, complexAttributesRelated):
    print '\t\tcreating mpi file: mpiCode/' + agentName + '_mpi.cxx for agent: ' + agentName + ' in namespace: ' + namespace + ' with parent: ' + parent + ' from source: ' + source + ' and header: ' + headerName
    f = open('mpiCode/' + agentName + '_mpi.cxx', 'w')
    # header
    f.write('\n')
    f.write('#include "' + agentName + '_mpi.hxx"\n')
    f.write('#include <' + agentName + '.hxx>\n')
    f.write('#include <cstring>\n')
    f.write('#include <iostream>\n')
    f.write('#include <mpi.h>\n')
    f.write('#include <vector>\n')
    f.write('#include <queue>\n')
    f.write('#include <map>\n')
    f.write('#include <Exception.hxx>\n')
    f.write('#include <typedefs.hxx>\n')
    f.write('\n')
    if namespace != "":
        f.write('namespace ' + namespace + '\n')
        f.write('{\n')
        f.write('\n')
    writeFillPackage(f, agentName, attributesMap)
    writeFreeAgentPackage(f, agentName)
    writeConstructor(f, agentName, parent, attributesMap)
    writeComparator(f, agentName, parent, attributesMap)
    writeVectorAttributesPassing(f, agentName, vectorAttributesMap)

    if len(complexAttributesRelated.complexAttributesOrderMap) > 0:
        getListOfSetterHeaders(agentName, complexAttributesRelated)

        writeComplexAttributesSettersHeaders(agentName, headerName, complexAttributesRelated)
        writeComplexAttributesSettersCode(f, agentName, complexAttributesRelated)
    
    writeCreateComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated)
    writeCopyContinuousValuesToDiscreteOnes(f, agentName, attributesMap, complexAttributesRelated)

    if namespace != "":
        f.write('} // namespace ' + namespace + '\n')
    f.write('\n')
    f.close()
    return None


# attributes defined as a std::vector of basic types
def addVectorAttribute(line, vectorAttributesMap):
    indexTemplateBegin = line.find('<')
    indexTemplateEnd = line.find('>')
    typeVector = line[indexTemplateBegin + 1:indexTemplateEnd]
    restOfLine = line[indexTemplateEnd + 1:]
    indexEndOfName = restOfLine.find(';')
    variableName = restOfLine[:indexEndOfName].strip()
    vectorAttributesMap[variableName] = typeVector
    print '\t\t\tvector attribute detected: ' + variableName + ' with type: std::vector of: ' + typeVector
    return None


# attributes with basic types (int, float, char, ...)
def addBasicAttribute(line, attributesMap):
    splitLine = line.split()

    # 1st word will be the type of the attribute
    typeAttribute = splitLine[0]

    # 2nd word will be the name, removing final ';'
    variableName = splitLine[1]
    variableName = variableName.strip(';')

    attributesMap[variableName] = typeAttribute
    print '\t\t\tattribute detected: ' + variableName + ' with type: ' + typeAttribute
    return None


def addStringAttribute(line, attributesMap):
    splitLine = line.split()

    # 1st word will be std::string
    typeAttribute = 'string'

    # 2nd word will be the name, removing final ';'
    variableName = splitLine[1]
    variableName = variableName.strip(';')

    attributesMap[variableName] = typeAttribute
    print '\t\t\tattribute detected: ' + variableName + ' with type: string'
    return None

def getNewIDFromOrderMap(variableName, complexAttributesRelated):
    newID = 1
    if len(complexAttributesRelated.complexAttributesOrderMap) > 0:
        lastID = max(complexAttributesRelated.complexAttributesOrderMap, key = int)
        newID = lastID + 1

    return newID

def addComplexAttribute(line, complexAttributesRelated):
    splitLine = line.split(";")
    firstPart = splitLine[0]
    firstPartSplit = firstPart.split()

    firstPartLength = len(firstPartSplit)
    
    # 1st strings will the the type of the attribute, even if they are separated by spaces.
    typeAttribute = ''
    for i in range(0, firstPartLength - 1):
        typeAttribute += firstPartSplit[i]

    # 2nd word will be the name, removing final ';'
    variableName = firstPartSplit[firstPartLength - 1]
    variableName = variableName.strip(';')

    newID = getNewIDFromOrderMap(variableName, complexAttributesRelated)

    complexAttributesRelated.complexAttributesOrderMap[newID] = variableName
    complexAttributesRelated.complexAttributesFullType[newID] = typeAttribute

    print '\t\t\tattribute detected: ' + variableName + ' with type: ' + typeAttribute

    return None

def getAttributesFromClass(className, attributesMap, vectorAttributesMap, complexAttributesRelated):
    headerName = className + '.hxx'
    print '\t\tlooking for attributes of class: ' + className + ' in header: ' + headerName + '...'
    f = open(headerName, 'r')
    keyBasic = 'MpiBasicAttribute'
    keyVector = 'MpiVectorAttribute'
    keyString = 'MpiStringAttribute'
    keyComplex = 'MpiComplexAttribute'
    for line in f:
        if line.find(keyBasic) != -1:
            addBasicAttribute(line, attributesMap)
        elif line.find(keyVector) != -1:
            addVectorAttribute(line, vectorAttributesMap)
        elif line.find(keyString) != -1:
            addStringAttribute(line, attributesMap)
        elif line.find(keyComplex) != -1:
            addComplexAttribute(line, complexAttributesRelated)
        # parse base class, it must inherit from Agent
        elif line.find('class') != -1 and line.find(className) != -1 and line.find('public') != -1:
            splittedLine = line.rsplit()
            parentName = splittedLine[len(splittedLine) - 1]
            # remove namespace in case it exists
            parentNameWithoutNamespace = parentName
            indexSeparator = parentName.find('::')
            if (indexSeparator != -1):
                parentNameWithoutNamespace = parentName[indexSeparator + 2:]
            if parentNameWithoutNamespace != 'Agent':
                getAttributesFromClass(parentNameWithoutNamespace, attributesMap, vectorAttributesMap, complexAttributesRelated)
    f.close()
    return parentName

def includeVirtualMethodsHeaders(agentName, headerName, parentName):
    print '\tchecking if header: ' + headerName + ' for agent: ' + agentName + ' defines needed virtual methods...'
    # if this is not defined, we will add the four needed methods
    fillPackageName = 'fillPackage'
    f = open(headerName, 'r')
    for line in f:
        if line.find(fillPackageName) != -1:
            print '\tVirtual methods headers at "' + headerName + '" already correct.'
            return
    f.close()

    print '\t"' + headerName + '" does not contain virtual methods headers, adding them...'

    headerNameTmp = headerName + '_tmp'
    f = open(headerName, 'r')
    fTmp = open(headerNameTmp, 'w')

    insideClass = 0
    for line in f:
        if insideClass == 0:
            if line.find('class') != -1 and line.find(agentName) != -1:
                print 'accessing agent declaration: ' + agentName
                insideClass = 1
            fTmp.write(line)
        elif line.find('};') != -1:
            print 'end of agent declaration: ' + agentName
            insideClass = 0
            fTmp.write('\n')
            fTmp.write('\t///// Autogenerated code (do not modify): /////\n')
            fTmp.write('public:\n')
            fTmp.write('\t' + agentName + '( void * );\n')
            fTmp.write('\tvoid* fillPackage() const override;\n')
            fTmp.write('\tvoid freePackage(void* package) const override;\n')
            fTmp.write('\tbool hasTheSameAttributes(const '+ parentName +'& other) const override;\n')
            fTmp.write('\tvoid sendVectorAttributes(int);\n')
            fTmp.write('\tvoid receiveVectorAttributes(int);\n')
            fTmp.write('\tvoid* createComplexAttributesDeltaPackage(int& sizeOfPackage) override;\n')
            fTmp.write('\tvoid copyContinuousValuesToDiscreteOnes() override;\n')
            fTmp.write('\t///////// End of autogenerated code ///////////\n')
            fTmp.write('\n')
            fTmp.write(line)
        else:
            fTmp.write(line)
    f.close()
    fTmp.close()
    os.rename(headerNameTmp, headerName)
    return None

def includeDiscreteVariables(agentName, headerName, attributesMap, complexAttributesRelated):
    print '\tchecking if header: ' + headerName + ' for agent: ' + agentName + ' defines needed discrete variables...'
    # if this is not defined, we will add the four needed methods
    firstDiscreteVariableName = '_discrete' + next(iter(attributesMap))
    f = open(headerName, 'r')
    for line in f:
        if line.find(firstDiscreteVariableName) != -1:
            print '\tDiscrete variables includes at "' + headerName + '" already correct.'
            return
    f.close()

    print '\t"' + headerName + '" does not contain discrete variables headers, adding them...'

    headerNameTmp = headerName + '_tmp'
    f = open(headerName, 'r')
    fTmp = open(headerNameTmp, 'w')

    insideClass = 0
    for line in f:
        if insideClass == 0:
            if line.find('class') != -1 and line.find(agentName) != -1:
                print 'accessing agent declaration: ' + agentName
                insideClass = 1
            fTmp.write(line)
        elif line.find('};') != -1:
            print 'end of agent declaration: ' + agentName
            insideClass = 0
            fTmp.write('\n')
            fTmp.write('\t///// Autogenerated code (do not modify): /////\n')
            fTmp.write('private:\n')
            for key in attributesMap:
                fTmp.write('\t' + attributesMap[key] + ' _discrete' + key + ';\n')
            
            fTmp.write('\n')
            
            for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
                fTmp.write('\t' + complexAttributesRelated.complexAttributesFullType[variableID] + ' _discrete' + variableName + ';\n')

            fTmp.write('\t///////// End of autogenerated code ///////////\n')
            fTmp.write('\n')
            fTmp.write(line)
        else:
            fTmp.write(line)
            
    f.close()
    fTmp.close()
    os.rename(headerNameTmp, headerName)

    return None

class ComplexAttributesRelated:
    
    def __init__(self):
        self.complexAttributesOrderMap = {}                      # <int: variableID, str: variableName>
        self.complexAttributesShortType = {}                     # <int: variableID, str: variableShortType>
        self.complexAttributesFullType = {}                      # <int: variableID, str: variableFullType>
        self.complexAttributesElementsType = {}                  # <int: variableID, str: variableElementsType>
        self.complexAttributesValueInMapType = {}                # <int: variableID, str: valuesInMapType>

        self.mapOfDeltaHeaders = collections.defaultdict(dict)   # <int: variableID, <str: methodType, str: methodHeaderWithParams>>
        self.mapOfDeltaVariablesName = {}                        # <int: variableID, str: deltaVariableName>
        self.mapOfDeltaVariablesType = {}                        # <int: variableID, str: deltaVariableType>
        self.mapOfDeltaVariablesComments = {}                    # <int: variableID, str: deltaVariableComments>

def execute(target, source, env):
    print 'generating code for mpi communication...'
    listAgents = []
    listAttributesMaps = []
    namespaceAgents = env['namespaces']
    for i in range(1, len(source)):
        sourceName = str(source[i])
        headerName = sourceName.replace(".cxx", ".hxx")
        agentName = sourceName.replace(".cxx", "")

        listAgents += [agentName]

        attributesMap = {}
        vectorAttributesMap = {}
        complexAttributesRelated = ComplexAttributesRelated()

        parentName = getAttributesFromClass(listAgents[i - 1], attributesMap, vectorAttributesMap, complexAttributesRelated)
        includeVirtualMethodsHeaders(agentName, headerName, parentName)
        includeDiscreteVariables(agentName, headerName, attributesMap, complexAttributesRelated)
        print '\tprocessing agent: ' + listAgents[i - 1]
        # get the list of attributes to send/receive in MPI
        # create header declaring a package with the list of attributes
        createMpiHeader(listAgents[i - 1], sourceName, headerName, attributesMap)
        # create a source code defining package-class copy
        createMpiCode(listAgents[i - 1], sourceName, headerName, namespaceAgents[i - 1], parentName, attributesMap,
                      vectorAttributesMap, complexAttributesRelated)
        listAttributesMaps.append(attributesMap)

    # fill mpi code registering types and additional methods
    createFactoryMethods(listAgents, str(target[0]), namespaceAgents, listAttributesMaps)
    print 'done!'
    return None
