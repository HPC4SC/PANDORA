#!/usr/bin/env python3
import os
import collections
import sys

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
    numDynamicParams = 0
    for nameAttribute, typeAttribute in attributesMap.items():
        if typeAttribute == "Engine::Point2D<int>":
            numDynamicParams += 2
        else:
            numDynamicParams += 1

    numAttributes = numBasicAttributes + numDynamicParams
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
        if typeAttribute == "std::string":
            f.write('\t// ' + nameAttribute + '\n')
            f.write('\tblockLengths[' + str(index) + '] = 32;\n')
            f.write('\ttypeList[' + str(index) + '] = MPI_CHAR;\n');
        elif typeAttribute == "Engine::Point2D<int>":
            f.write('\t// ' + nameAttribute + '._x\n')
            f.write('\tblockLengths[' + str(index) + '] = 1;\n')
            f.write('\ttypeList[' + str(index) + '] = MPI_INT;\n')
            index += 1
            f.write('\t// ' + nameAttribute + '._y\n')
            f.write('\tblockLengths[' + str(index) + '] = 1;\n')
            f.write('\ttypeList[' + str(index) + '] = MPI_INT;\n')
        else:
            f.write('\t// ' + nameAttribute + '\n')
            f.write('\tblockLengths[' + str(index) + '] = 1;\n')
            mpiTypeAttribute = getMpiTypeAttribute(typeAttribute)
            f.write('\ttypeList[' + str(index) + '] = ' + mpiTypeAttribute + ';\n')
        index += 1
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
        if typeAttribute == "Engine::Point2D<int>":
            f.write('\t// ' + nameAttribute + '._x\n')
            f.write('\tMPI_Address(&package.' + nameAttribute + 'Mpi._x, &address);\n')
            f.write('\tdisplacements[' + str(index) + '] = address-startAddress;\n')
            index += 1
            f.write('\t// ' + nameAttribute + '._y\n')
            f.write('\tMPI_Address(&package.' + nameAttribute + 'Mpi._y, &address);\n')
            f.write('\tdisplacements[' + str(index) + '] = address-startAddress;\n')
        else:
            f.write('\t// ' + nameAttribute + '\n')
            f.write('\tMPI_Address(&package.' + nameAttribute + 'Mpi, &address);\n')
            f.write('\tdisplacements[' + str(index) + '] = address-startAddress;\n')
        index += 1

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
        if typeAttribute == "std::string":
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
        if typeAttribute == "std::string":
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
    updateMethodCode =  '\tif (index >= (int) ' + variableName + '.size() or index < 0) throw Engine::Exception(CreateStringStream("' + agentName + '_mpi.cxx::updateElementAtIndex' + variableName + '() - NOT VALID INDEX: " << index << ".").str());\n' \
                        '\n' \
                        '\t' + variableName + '.at(index) = element;\n' \
                        '\n' \
                        '\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eVectorUpdate, index, element));\n'

    typeOfElementsNull = '(' + typeOfElements + ') 0'
    if typeOfElements.find('Engine::Point2D') != -1:
        typeOfElementsNull = 'Engine::Point2D<int>()'
    elif typeOfElements.find('std::string') != -1:
        typeOfElementsNull = '(' + typeOfElements + ') ""'

    deleteMethodCode =  '\tif (index >= (int) ' + variableName + '.size() or index < -1) throw Engine::Exception(CreateStringStream("' + agentName + '_mpi.cxx::deleteElementAtIndex' + variableName + '() - NOT VALID INDEX: " << index << ".").str());\n' \
                        '\n' \
                        '\tif (index == -1) ' + variableName + '.clear();\n' \
                        '\telse ' + variableName + '.erase(' + variableName + '.begin() + index);\n' \
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
    elif typeOfElements.find('std::string') != -1:
        typeOfElementsNull = '(' + typeOfElements + ') ""'

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
    elif valueType.find('std::string') != -1:
        valueTypeNull = '(' + valueType + ') ""'

    deleteMethodCode =          '\tif (' + variableName + '.erase(key) != 0 )\n' \
                                '\t\t' + deltaVariableName + '.push_back(std::make_tuple(Engine::eMapDelete, key, ' + valueTypeNull + '));\n'

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
        elif complexAttributesRelated.complexAttributesFullType[variableID].find('<char') != -1:
            typeOfElements = 'char'
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
            elif complexAttributesRelated.complexAttributesFullType[variableID].find(',char') != -1:
                valueType = 'char'
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

def writeGetComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated):
    f.write('void* ' + agentName + '::getComplexAttributesDeltaPackage(int& sizeOfPackage)\n')
    f.write('{\n')
    if len(complexAttributesRelated.complexAttributesOrderMap) > 0:
        f.write('\tsizeOfPackage = *((int*) _deltaPackage);\n')
        f.write('\treturn _deltaPackage;\n')
    else:
        f.write('\t return NULL;')
    f.write('}\n')
    f.write('\n')
    return None

def setDeltaStringsIfNecessary(f, complexAttributesRelated, deltaSizeVariables, deltaStringSize, deltaStringSize_valueInMap):
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]
        dsShortType = complexAttributesRelated.complexAttributesShortType[variableID]
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

        if typeOfElements == 'std::string':
            if dsShortType == 'std::vector': elementsIndexInDeltaDS = '2'
            elif dsShortType == 'std::queue' or dsShortType == 'std::map': elementsIndexInDeltaDS = '1'

            deltaStringSize[variableID] = 'delta' + variableName + 'ValueStringSize'
            f.write('\tint ' + deltaStringSize[variableID] + ' = 0;\n')
            f.write('\tfor (int i = 0; i < ' + deltaSizeVariables[variableID] + '; ++i)\n')
            f.write('\t\t' + deltaStringSize[variableID] + ' += std::get<' + elementsIndexInDeltaDS + '>(' + deltaVariableName + '[i]).length();\n')
            f.write('\n')

        if dsShortType == 'std::map':
            valueInMapType = complexAttributesRelated.complexAttributesValueInMapType[variableID]
            if valueInMapType == 'std::string':
                deltaStringSize_valueInMap[variableID] = 'delta' + variableName + 'ValueStringSize_valueInMap'
                f.write('\tint ' + deltaStringSize_valueInMap[variableID] + ' = 0;\n')
                f.write('\tfor (int i = 0; i < ' + deltaSizeVariables[variableID] + '; ++i)\n')
                f.write('\t\t' + deltaStringSize_valueInMap[variableID] + ' += std::get<2>(' + deltaVariableName + '[i]).length();\n')
                f.write('\n')
    return None

def setDeltaNumberOfBytesVariables(f, complexAttributesRelated, deltaSizeVariables, deltaStringSize, deltaStringSize_valueInMap, deltaNumberOfBytesVariables):
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]
        dsShortType = complexAttributesRelated.complexAttributesShortType[variableID]

        deltaNumberOfBytesVariables[variableID] = 'delta' + variableName + 'ElementsListNumberOfBytes'
        f.write('\tint ' + deltaNumberOfBytesVariables[variableID] + ' = ' + deltaSizeVariables[variableID] + ' * sizeof(int)')

        if dsShortType == 'std::vector': f.write(' + ' + deltaSizeVariables[variableID] + ' * sizeof(int)')

        if typeOfElements == 'Engine::Point2D<int>':
            f.write(' + ' + deltaSizeVariables[variableID] + ' * 2 * sizeof(int)')
        elif typeOfElements == 'std::string':
            f.write(' + ' + deltaSizeVariables[variableID] + ' * sizeof(int) + ' + deltaStringSize[variableID])
        else:
            f.write(' + ' + deltaSizeVariables[variableID] + ' * sizeof(' + typeOfElements + ')')
        
        if dsShortType == 'std::map':
            valueInMapType = complexAttributesRelated.complexAttributesValueInMapType[variableID]

            if valueInMapType == 'Engine::Point2D<int>':
                f.write(' + ' + deltaSizeVariables[variableID] + ' * 2 * sizeof(int)')
            elif valueInMapType == 'std::string':
                f.write(' + ' + deltaSizeVariables[variableID] + ' * sizeof(int) + ' + deltaStringSize_valueInMap[variableID])
            else:
                f.write(' + ' + deltaSizeVariables[variableID] + ' * sizeof(' + valueInMapType + ')')
        
        f.write(';\n')

    return None

def writePoints2DVariablesDefinitionParametrized_sender(f, typeOfElements, dsVariableName):
    if typeOfElements == 'Engine::Point2D<int>':
        f.write('\n')
        f.write('\t\tint ' + dsVariableName + 'X = ' + dsVariableName + '.getX();\n')
        f.write('\t\tint ' + dsVariableName + 'Y = ' + dsVariableName + '.getY();\n')

    return None

def writePoints2DOrStringMemCpyParametrized(f, typeOfElements, dsVariableName):
    if typeOfElements == 'Engine::Point2D<int>':
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + 'X, sizeof(int));\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + 'Y, sizeof(int));\t\tbyteIndex += sizeof(int);\n')
    elif typeOfElements == 'std::string':
        f.write('\n')
        f.write('\t\tint ' + dsVariableName + 'Length = ' + dsVariableName + '.length();\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + 'Length, sizeof(int));\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\tfor (int j = 0; j < ' + dsVariableName + 'Length; ++j)\n')
        f.write('\t\t{\n')
        f.write('\t\t\tmemcpy((char*) resultingPackage + byteIndex, &(' + dsVariableName + '.at(j)), sizeof(char));\t\t\tbyteIndex += sizeof(char);\n')
        f.write('\t\t}\n')
    else:
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + dsVariableName + ', sizeof(' + typeOfElements + '));\t\t\tbyteIndex += sizeof(' + typeOfElements + ');\n')
    
    return None

def writeCreateComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated):
    f.write('int ' + agentName + '::createComplexAttributesDeltaPackage()\n')
    f.write('{\n')

    if len(complexAttributesRelated.complexAttributesOrderMap) == 0:
        f.write('\treturn 0;\n')
        f.write('}\n')
        f.write('\n')
        return None

    f.write('//std::cout << CreateStringStream("Bug " << getId() << " in createComplexAttributesDeltaPackage()\\n").str();\n')

    deltaSizeVariables = {}
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

        deltaSizeVariable = 'delta' + variableName + 'Size'
        f.write('\tint ' + deltaSizeVariable + ' = ' + deltaVariableName + '.size();\n')

        deltaSizeVariables[variableID] = deltaSizeVariable
    
    f.write('\n')

    deltaStringSize, deltaStringSize_valueInMap = {}, {}
    setDeltaStringsIfNecessary(f, complexAttributesRelated, deltaSizeVariables, deltaStringSize, deltaStringSize_valueInMap)

    deltaNumberOfBytesVariables = {}
    setDeltaNumberOfBytesVariables(f, complexAttributesRelated, deltaSizeVariables, deltaStringSize, deltaStringSize_valueInMap, deltaNumberOfBytesVariables)

    f.write('\n')
    f.write('\tint agentIDLength = getId().length();\n')
    f.write('\n')

    f.write('\t// <packageLength, agentID, list<variableID, elementsListNumberOfBytes, list<typeOfModification, [elementIndex,] elementNewValue>>>\n')
    f.write('\t//\t\tagentID = sizeof(int) + agentIDLength * sizeof(char)\n')
    f.write('\t//\t\tif elementNewValue == Engine::Point2D<int> -> elementNewValue = <elementNewValue.X, elementNewValue.Y>\n')
    f.write('\t//\t\tif elementNewValue == std::string -> elementNewValue = <elementNewValue.length, elementNewValue.listOfChars>\n')

    f.write('\tint sizeOfPackage =\tsizeof(int) + sizeof(int) + agentIDLength * sizeof(char) +\n')
    for variableID, variableShortType in complexAttributesRelated.complexAttributesShortType.items():
        f.write('\t\t\t\t\t\t(' + deltaSizeVariables[variableID] + ' > 0) * 2 * sizeof(int) + ' + deltaNumberOfBytesVariables[variableID])

        if variableID != complexAttributesRelated.complexAttributesShortType.keys()[-1]:
            f.write(' +\n')
        else:
            f.write(';\n')

    f.write('\n')
    f.write('\tvoid* resultingPackage = malloc(sizeOfPackage);\n')
    f.write('\t_deltaPackage = resultingPackage;\n')
    f.write('\n')
    f.write('\tint byteIndex = 0;\n')
    f.write('\tint variableID;\n')
    f.write('\n')
    f.write('\tmemcpy((char*) resultingPackage + byteIndex, &sizeOfPackage, sizeof(int));\t\t\t\t\t\tbyteIndex += sizeof(int);\n')
    f.write('\tmemcpy((char*) resultingPackage + byteIndex, &agentIDLength, sizeof(int));\t\t\t\t\t\tbyteIndex += sizeof(int);\n')
    f.write('\tfor (int i = 0; i < getId().length(); ++i)\n')
    f.write('\t{\tmemcpy((char*) resultingPackage + byteIndex, &(getId().at(i)), sizeof(char));\t\t\t\tbyteIndex += sizeof(char); }\n')
    f.write('\n')

    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]
        variableShortType = complexAttributesRelated.complexAttributesShortType[variableID]
        typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]

        f.write('\t// Delta elements for variable ' + variableName + ':\n')
        f.write('\tif (' + deltaSizeVariables[variableID] + ' > 0)\n')
        f.write('\t{\n')
        f.write('\t\tvariableID = ' + str(variableID) + ';\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &variableID, sizeof(int));\t\t\t\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &' + deltaNumberOfBytesVariables[variableID] + ', sizeof(int));\t\t\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t}\n')

        f.write('\tfor (int i = 0; i < ' + deltaSizeVariables[variableID] + '; ++i)\n')
        f.write('\t{\n')
        f.write('\t\tint typeOfModification = std::get<0>(' + deltaVariableName + '[i]);\n')
        if variableShortType == 'std::vector':
            f.write('\t\tint elementIndex = std::get<1>(' + deltaVariableName + '[i]);\n')
            f.write('\t\t' + typeOfElements + ' elementNewValue = std::get<2>(' + deltaVariableName + '[i]);\n')

            f.write('//std::cout << CreateStringStream("hey1 - typeOfModification: " << typeOfModification << " elementIndex: " << elementIndex << " elementNewValue: " << elementNewValue << "\\n").str();\n')
        elif variableShortType == 'std::queue':
            f.write('\t\t' + typeOfElements + ' elementNewValue = std::get<1>(' + deltaVariableName + '[i]);\n')
        elif variableShortType == 'std::map':
            valueType = complexAttributesRelated.complexAttributesValueInMapType[variableID]

            f.write('\t\t' + typeOfElements + ' elementKey = std::get<1>(' + deltaVariableName + '[i]);\n')
            f.write('\t\t' + valueType + ' elementNewValue = std::get<2>(' + deltaVariableName + '[i]);\n')


        if variableShortType == 'std::map':
            writePoints2DVariablesDefinitionParametrized_sender(f, typeOfElements, 'elementKey')
            writePoints2DVariablesDefinitionParametrized_sender(f, valueType, 'elementNewValue')
        else:
            writePoints2DVariablesDefinitionParametrized_sender(f, typeOfElements, 'elementNewValue')

        f.write('\n')
        f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &typeOfModification, sizeof(int));\t\tbyteIndex += sizeof(int);\n')

        if variableShortType == 'std::map':
            writePoints2DOrStringMemCpyParametrized(f, typeOfElements, 'elementKey')
            writePoints2DOrStringMemCpyParametrized(f, valueType, 'elementNewValue')
        else:
            if variableShortType == 'std::vector':
                f.write('\t\tmemcpy((char*) resultingPackage + byteIndex, &elementIndex, sizeof(int));\t\t\tbyteIndex += sizeof(int);\n')
            writePoints2DOrStringMemCpyParametrized(f, typeOfElements, 'elementNewValue')

        f.write('\t}\n')
    f.write('//std::cout << CreateStringStream("Package " << getId() << " created in createComplexAttributesDeltaPackage(): " << *((int*) (resultingPackage + 0)) << " where sizeOfPackage: " << sizeOfPackage << "\\n").str();\n')

    f.write('\n')
    for variableID in complexAttributesRelated.complexAttributesOrderMap:
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]
        f.write('\t' + deltaVariableName + '.clear();\n')
    f.write('\n')

    f.write('\treturn sizeOfPackage;\n')
    f.write('}\n')
    f.write('\n')
    return None

def writePoints2DVariablesDefinitionParametrized_receiver(f, typeOfElements, dsVariableName):
    if typeOfElements == 'Engine::Point2D<int>':
        f.write('\n')
        f.write('\t\t\tint ' + dsVariableName + 'X = *((int*) (package + byteIndex));\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\t\tint ' + dsVariableName + 'Y = *((int*) (package + byteIndex));\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\t\tEngine::Point2D<int> ' + dsVariableName + ' = Engine::Point2D<int>(' + dsVariableName + 'X, ' + dsVariableName + 'Y);\n')
    elif typeOfElements == 'std::string':
        f.write('\n')
        f.write('\t\t\tstd::string ' + dsVariableName + ' = "";\n')
        f.write('\t\t\tint ' + dsVariableName + 'Length = *((int*) (package + byteIndex));\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\t\tfor (int i = 0; i < ' + dsVariableName + 'Length; ++i)\n')
        f.write('\t\t\t{\n')
        f.write('\t\t\t\tchar charI = *((char*) (package + byteIndex));\t\t\tbyteIndex += sizeof(char);\n')
        f.write('\t\t\t\t' + dsVariableName + ' += charI;\n')
        f.write('\t\t\t}\n')
    else:
        f.write('\t\t\t' + typeOfElements + ' ' + dsVariableName + ' = *((' + typeOfElements + '*) (package + byteIndex));\t\t\tbyteIndex += sizeof(' + typeOfElements + ');\n')

    return None

def writePrintVector(f, variableName):
    f.write('\n')
    #f.write('\t\tif (_world->getConfig().getPrintInConsole())\n')
    #f.write('\t\t{\n')
    f.write('\t\t\tstd::cout << CreateStringStream("[applyComplexAttributesDeltaPackage()] AGENT: " << getId() << "\\t' + variableName + ': ").str();\n')
    f.write('\t\t\tfor (int i = 0; i < ' + variableName + '.size(); ++i)\n')
    f.write('\t\t\t\tstd::cout << CreateStringStream(' + variableName + '[i] << ", ").str();\n\n')
    f.write('\t\t\tstd::cout << CreateStringStream("\\n").str();\n')
    #f.write('\t\t}\n')
    f.write('\n')
    return None

def writePrintQueue(f, variableName, typeOfElements):
    f.write('\n')
    #f.write('\t\tif (_world->getConfig().getPrintInConsole())\n')
    #f.write('\t\t{\n')
    f.write('\t\t\tstd::queue<' + typeOfElements + '> ' + variableName + 'Aux = ' + variableName + ';\n')
    f.write('\t\t\tstd::cout << CreateStringStream("[applyComplexAttributesDeltaPackage()] AGENT: " << getId() << "\\t' + variableName + ': ").str();\n')
    f.write('\t\t\twhile (not ' + variableName + 'Aux.empty())\n')
    f.write('\t\t\t{\tstd::cout << CreateStringStream(' + variableName + 'Aux.front() << ", ").str();\t' + variableName + 'Aux.pop(); }\n\n')
    f.write('\t\t\tstd::cout << CreateStringStream("\\n").str();\n')
    #f.write('\t\t}\n')
    f.write('\n')
    return None

def writePrintMap(f, variableName, typeOfElements, valueType):
    f.write('\n')
    #f.write('\t\tif (_world->getConfig().getPrintInConsole())\n')
    #f.write('\t\t{\n')
    f.write('\t\t\tstd::cout << CreateStringStream("[applyComplexAttributesDeltaPackage()] AGENT: " << getId() << "\\t' + variableName + ': ").str();\n')
    f.write('\t\t\tfor (std::map<' + typeOfElements + ', ' + valueType + '>::const_iterator it = ' + variableName + '.begin(); it != ' + variableName + '.end(); ++it)\n')
    f.write('\t\t\t\tstd::cout << CreateStringStream("<" << it->first << "," << it->second << ">, ").str();\n\n')
    f.write('\t\t\tstd::cout << CreateStringStream("\\n").str();\n')
    #f.write('\t\t}\n')
    f.write('\n')
    return None

def applyChangesToVector(f, variableName, typeOfElements, numberOfIdentations):
    baseIdentation = ''
    for i in range(numberOfIdentations):
        baseIdentation += '\t'

    f.write(baseIdentation + 'if (typeOfModification == Engine::eVectorInsert)\n')
    f.write(baseIdentation + '{\n')
    f.write(baseIdentation + '\tif (elementIndex >= ' + variableName + '.size() or elementIndex < 0) ' + variableName + '.push_back(elementNewValue);\n')
    f.write(baseIdentation + '\telse\n')
    f.write(baseIdentation + '\t{\n')
    f.write(baseIdentation + '\t\tstd::vector<' + typeOfElements + '>::const_iterator itPos = ' + variableName + '.begin() + elementIndex;\n')
    f.write(baseIdentation + '\t\t' + variableName + '.insert(itPos, elementNewValue);\n')
    f.write(baseIdentation + '\t}\n')
    f.write(baseIdentation + '}\n')
    f.write(baseIdentation + 'else if (typeOfModification == Engine::eVectorUpdate)\n')
    f.write(baseIdentation + '\t' + variableName + '.at(elementIndex) = elementNewValue;\n')
    f.write(baseIdentation + 'else if (typeOfModification == Engine::eVectorDelete)\n')
    f.write(baseIdentation + '{\n')
    f.write(baseIdentation + '\tif (elementIndex == -1) ' + variableName + '.clear();\n')
    f.write(baseIdentation + '\telse ' + variableName + '.erase(' + variableName + '.begin() + elementIndex);\n')
    f.write(baseIdentation + '}\n')

    return None

def applyChangesToQueue(f, variableName, numberOfIdentations):
    baseIdentation = ''
    for i in range(numberOfIdentations):
        baseIdentation += '\t'

    f.write(baseIdentation + 'if (typeOfModification == Engine::eQueuePush)\n')
    f.write(baseIdentation + '\t' + variableName + '.push(elementNewValue);\n')
    f.write(baseIdentation + 'if (typeOfModification == Engine::eQueuePop)\n')
    f.write(baseIdentation + '\t' + variableName + '.pop();\n')

    return None

def applyChangesToMap(f, variableName, numberOfIdentations):
    baseIdentation = ''
    for i in range(numberOfIdentations):
        baseIdentation += '\t'

    f.write(baseIdentation + 'if (typeOfModification == Engine::eMapInsertOrUpdate)\n')
    f.write(baseIdentation + '\t' + variableName + '[elementKey] = elementNewValue;\n')
    f.write(baseIdentation + 'if (typeOfModification == Engine::eMapDelete)\n')
    f.write(baseIdentation + '\t' + variableName + '.erase(elementKey);\n')

    return None

def writeApplyComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated):
    f.write('void ' + agentName + '::applyComplexAttributesDeltaPackage(void* package)\n')
    f.write('{\n')

    if len(complexAttributesRelated.complexAttributesOrderMap) == 0:
        f.write('}\n')
        f.write('\n')
        return None

    f.write('\tint byteIndex = 0;\n')
    f.write('\tint packageSize = *((int*) (package + byteIndex));\t\t\t\t\t\tbyteIndex += sizeof(int);\n')
    f.write('\n')
    f.write('\tint sizeOfAgentIDString = *((int*) (package + byteIndex));\t\t\t\tbyteIndex += sizeof(int) + sizeOfAgentIDString * sizeof(char);\n')
    f.write('\n')
    f.write('\tint deltaVariableID, deltaVariableNumberOfBytes, finalPositionForVariable;\n')
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        variableShortType = complexAttributesRelated.complexAttributesShortType[variableID]
        typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]

        f.write('\tif (byteIndex < packageSize)\n')
        f.write('\t{\n')

        f.write('\t\t// Delta elements for variable ' + variableName + ':\n')
        f.write('\t\tdeltaVariableID = *((int*) (package + byteIndex));\t\t\t\t\tbyteIndex += sizeof(int);\n')
        f.write('\t\tdeltaVariableNumberOfBytes = *((int*) (package + byteIndex));\t\tbyteIndex += sizeof(int);\n')
        f.write('\n')
        f.write('\nif (_world->getConfig().getPrintInConsole()) std::cout << CreateStringStream("[applyComplexAttributesDeltaPackage()] AGENT: " << getId() << " ' + variableName + ' - byteIndex: " << byteIndex << " deltaVariableNumberOfBytes: " << deltaVariableNumberOfBytes << "\\n").str();\n\n')
        f.write('\t\tfinalPositionForVariable = byteIndex + deltaVariableNumberOfBytes;\n')
        f.write('\t\twhile (byteIndex < finalPositionForVariable)\n')
        f.write('\t\t{\n')
        f.write('\t\t\tint typeOfModification = *((int*) (package + byteIndex));\t\tbyteIndex += sizeof(int);\n')
        if variableShortType == 'std::vector':
            f.write('\t\t\tint elementIndex = *((int*) (package + byteIndex));\t\t\t\tbyteIndex += sizeof(int);\n')
        elif variableShortType == 'std::map':
            valueType = complexAttributesRelated.complexAttributesValueInMapType[variableID]

        if variableShortType == 'std::map':
            writePoints2DVariablesDefinitionParametrized_receiver(f, typeOfElements, 'elementKey')
            writePoints2DVariablesDefinitionParametrized_receiver(f, valueType, 'elementNewValue')
        else:
            writePoints2DVariablesDefinitionParametrized_receiver(f, typeOfElements, 'elementNewValue')

        if variableShortType == 'std::vector':
            applyChangesToVector(f, variableName, typeOfElements, 3)
        elif variableShortType == 'std::queue':
            applyChangesToQueue(f, variableName, 3)
        elif variableShortType == 'std::map':
            applyChangesToMap(f, variableName, 3)

        f.write('\t\t}\n')

        #if variableShortType == 'std::vector': writePrintVector(f, variableName)
        #elif variableShortType == 'std::queue': writePrintQueue(f, variableName, typeOfElements)
        #elif variableShortType == 'std::map': writePrintMap(f, variableName, typeOfElements, valueType)

        f.write('\t}\n')
        
        f.write('\n')

    f.write('}\n')
    f.write('\n')
    return None

def writeFreeComplexAttributeDeltaPackage(f, agentName, complexAttributesRelated):
    f.write('void ' + agentName + '::freeComplexAttributesDeltaPackage()\n')
    f.write('{\n')
    if len(complexAttributesRelated.complexAttributesOrderMap) > 0:
        f.write('\tfree(_deltaPackage);\n')
    f.write('}\n')
    return None

def writeCopyContinuousValuesToDiscreteOnes(f, agentName, attributesMap, complexAttributesRelated):
    f.write('void ' + agentName + '::copyContinuousValuesToDiscreteOnes()\n')
    f.write('{\n')
    f.write('\tEngine::Agent::copyContinuousValuesToDiscreteOnes();\n')
    f.write('\n')
    for key in attributesMap:
        f.write('\t_discrete' + key + ' = ' + key + ';\n')
    for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
        variableShortType = complexAttributesRelated.complexAttributesShortType[variableID]
        typeOfElements = complexAttributesRelated.complexAttributesElementsType[variableID]
        deltaVariableName = complexAttributesRelated.mapOfDeltaVariablesName[variableID]

        discreteVariableName = '_discrete' + variableName

        f.write('\n')
        f.write('\t// Applying delta elements for discrete variable ' + discreteVariableName + ':\n')
        f.write('\tfor (int i = 0; i < ' + deltaVariableName + '.size(); ++i)\n')
        f.write('\t{\n')

        f.write('\t\tint typeOfModification = std::get<0>(' + deltaVariableName + '[i]);\n')
        if variableShortType == 'std::vector':
            f.write('\t\tint elementIndex = std::get<1>(' + deltaVariableName + '[i]);\n')
            f.write('\t\t' + typeOfElements + ' elementNewValue = std::get<2>(' + deltaVariableName + '[i]);\n')
        elif variableShortType == 'std::queue':
            f.write('\t\t' + typeOfElements + ' elementNewValue = std::get<1>(' + deltaVariableName + '[i]);\n')
        elif variableShortType == 'std::map':
            valueType = complexAttributesRelated.complexAttributesValueInMapType[variableID]

            f.write('\t\t' + typeOfElements + ' elementKey = std::get<1>(' + deltaVariableName + '[i]);\n')
            f.write('\t\t' + valueType + ' elementNewValue = std::get<2>(' + deltaVariableName + '[i]);\n')

        f.write('\n')
        if variableShortType == 'std::vector':
            applyChangesToVector(f, discreteVariableName, typeOfElements, 2)
        elif variableShortType == 'std::queue':
            applyChangesToQueue(f, discreteVariableName, 2)
        elif variableShortType == 'std::map':
            applyChangesToMap(f, discreteVariableName, 2)

        f.write('\t}\n')

    f.write('}\n')
    f.write('\n')
    return None

def createMpiCode(agentName, source, headerName, namespace, parent, attributesMap, complexAttributesRelated):
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

    if len(complexAttributesRelated.complexAttributesOrderMap) > 0:
        getListOfSetterHeaders(agentName, complexAttributesRelated)

        writeComplexAttributesSettersHeaders(agentName, headerName, complexAttributesRelated)
        writeComplexAttributesSettersCode(f, agentName, complexAttributesRelated)
    
    writeCreateComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated)
    writeGetComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated)
    writeApplyComplexAttributesDeltaPackage(f, agentName, complexAttributesRelated)
    writeFreeComplexAttributeDeltaPackage(f, agentName, complexAttributesRelated)
    writeCopyContinuousValuesToDiscreteOnes(f, agentName, attributesMap, complexAttributesRelated)

    if namespace != "":
        f.write('} // namespace ' + namespace + '\n')
    f.write('\n')
    f.close()
    return None

# MpiBasicAttribute: attributes with basic types (int, float, char, bool, std::string and Engine::Point2D<int>)
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

def getNewIDFromOrderMap(variableName, complexAttributesRelated):
    newID = 1
    if len(complexAttributesRelated.complexAttributesOrderMap) > 0:
        lastID = max(complexAttributesRelated.complexAttributesOrderMap, key = int)
        newID = lastID + 1

    return newID

# MpiComplexAttribute: attributes with complex types (std::vector<T>, std::queue<T>, std::map<T, U> ; T|U = {int, float, char, bool, std::string, Engine::Point2D<int>})
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

def getAttributesFromClass(className, attributesMap, complexAttributesRelated):
    headerName = className + '.hxx'
    print '\t\tlooking for attributes of class: ' + className + ' in header: ' + headerName + '...'
    f = open(headerName, 'r')
    keyBasic = 'MpiBasicAttribute'
    keyComplex = 'MpiComplexAttribute'
    for line in f:
        splitLineDoubleSlash = line.split("//")
        splitLineSlashAsterisk = line.split("/*")
        if  splitLineDoubleSlash[0].replace(" ", "").replace("\t", "") == "" or \
            splitLineSlashAsterisk[0].replace(" ", "").replace("\t", "") == "": 
                continue

        if line.find(keyBasic) != -1:
            addBasicAttribute(line, attributesMap)
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
                getAttributesFromClass(parentNameWithoutNamespace, attributesMap, complexAttributesRelated)
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
            fTmp.write('\tint createComplexAttributesDeltaPackage() override;\n')
            fTmp.write('\tvoid* getComplexAttributesDeltaPackage(int& sizeOfPackage) override;\n')
            fTmp.write('\tvoid applyComplexAttributesDeltaPackage(void* package) override;\n')
            fTmp.write('\tvoid freeComplexAttributesDeltaPackage() override;\n')
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

def checkIfAttributesHasChanged(agentName, headerName, attributesMap, complexAttributesRelated):
    if len(complexAttributesRelated.complexAttributesOrderMap) == 0: return None

    print '\tchecking if header: ' + headerName + ' for agent: ' + agentName + ' defines correct complex attributes auxiliary variables...'
    # if this is not defined, we will add the four needed methods

    numberOfCurrentAttributes = 0
    correctAttributes = {}
    correctComplexVariables = {}
    f = open(headerName, 'r')
    for line in f:
        if line.find(' _discrete') != -1:
            numberOfCurrentAttributes += 1
        
        for variableName, variableType in attributesMap.items():
            if line.find('\t' + variableType + ' _discrete' + variableName + ';') != -1:
                correctAttributes[variableName] = "OK"

        for variableID, variableName in complexAttributesRelated.complexAttributesOrderMap.items():
            fullType = complexAttributesRelated.complexAttributesFullType[variableID]

            if line.find('\t' + fullType + ' _discrete' + variableName + ';') != -1:
                correctComplexVariables[variableID] = "OK"

    f.close()

    numberOfAttributesToActuallyConsider = len(attributesMap) + len(complexAttributesRelated.complexAttributesOrderMap)
    if  numberOfCurrentAttributes == numberOfAttributesToActuallyConsider and \
        len(correctAttributes) == len(attributesMap) and len(correctComplexVariables) == len(complexAttributesRelated.complexAttributesOrderMap):
            print '\tDefinition of complex attributes auxiliary variables "' + headerName + '" already correct.'
            return

    print '\t"' + headerName + '" does not defines complex attributes auxiliary variables, erasing the current configuration...'

    headerNameTmp = headerName + '_tmp'
    f = open(headerName, 'r')
    fTmp = open(headerNameTmp, 'w')

    maintainingLines = True
    for line in f:
        if line.find('Autogenerated code (do not modify)') != -1:
            maintainingLines = False
            pass

        if maintainingLines:
            fTmp.write(line)
        
        if line.find('End of autogenerated code') != -1:
            maintainingLines = True

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

            fTmp.write('\tvoid* _deltaPackage;\n')
            
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
        complexAttributesRelated = ComplexAttributesRelated()

        parentName = getAttributesFromClass(listAgents[i - 1], attributesMap, complexAttributesRelated)
        checkIfAttributesHasChanged(agentName, headerName, attributesMap, complexAttributesRelated)
        includeVirtualMethodsHeaders(agentName, headerName, parentName)
        includeDiscreteVariables(agentName, headerName, attributesMap, complexAttributesRelated)
        print '\tprocessing agent: ' + listAgents[i - 1]
        # get the list of attributes to send/receive in MPI
        # create header declaring a package with the list of attributes
        if not os.path.exists('mpiCode/'): os.makedirs('mpiCode/')
        createMpiHeader(listAgents[i - 1], sourceName, headerName, attributesMap)
        # create a source code defining package-class copy
        createMpiCode(listAgents[i - 1], sourceName, headerName, namespaceAgents[i - 1], parentName, attributesMap, complexAttributesRelated)
        listAttributesMaps.append(attributesMap)

    # fill mpi code registering types and additional methods
    createFactoryMethods(listAgents, str(target[0]), namespaceAgents, listAttributesMaps)
    print 'done!'
    return None
