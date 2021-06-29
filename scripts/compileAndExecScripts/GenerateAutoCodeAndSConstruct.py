import os, sys
import subprocess

pandoraPath = os.getenv('PANDORAPATH')
sys.path.append(pandoraPath + '/bin')

import generateMpi
import generateCP

def readParameters(agents, namespaceAgents, world, srcFiles):
	print 'Reading the parameters file...'

	with open('compilingParameters.txt') as file:
		line = file.readline()

		while line:
			keyAndValue = line.split(' = ')

			if len(keyAndValue) == 2:
				key = keyAndValue[0]
				value = keyAndValue[1]

				if key == 'agents':
					agents += value.replace(' ', '').replace('\r', '').replace('\n', '').split(',')
				elif key == 'namespaceAgents':
					namespaceAgents += value.replace(' ', '').replace('\r', '').replace('\n', '').split(',')
				elif key == 'world':
					world += value.replace(' ', '').replace('\r', '').replace('\n', '').split(',')
				elif key == 'srcFiles':
					srcFiles += value.replace(' ', '').replace('\r', '').replace('\n', '').split(',')

			line = file.readline()

	print 'Detected the following parameters:'
	print agents
	print namespaceAgents
	print world[0]
	print srcFiles
	print '\n'

	return None

def generateMpiAndCheckpointingCode(agents, namespaceAgents):
	print 'Generating the MPI Code...'

	mpiAgentsSrc = ['mpiCode/FactoryCode.cxx']
	cpAgentsSrc = ['checkpointingCode/CheckpointingFactoryCode.cxx']
	agentsSrc = ['main.cxx']
	for agent in agents:
		if agent != '':
			agentsSrc.append(agent + '.cxx')
			mpiAgentsSrc.append('mpiCode/' + agent + '_mpi.cxx')
			cpAgentsSrc.append('checkpoingintCode/' + agent + '_checkpointing.cxx')

	env = {}
	env['namespaces'] = namespaceAgents

	generateMpi.execute(mpiAgentsSrc, agentsSrc, env)
	generateCP.execute(cpAgentsSrc, agentsSrc, env)

	print 'Done!'
	print '\n'

	return None

def generateSConstruct(agents, namespaceAgents, world, srcFiles):
	print 'Generating the SConstruct file...'

	fileName = 'SConstruct'
	fileTmpName = fileName + '_tmp'

	file = open(fileName, 'r')
	fileTmp = open(fileTmpName, 'w')

	for line in file:
		if line.find('agents = [') != -1:
			fileTmp.write('agents = [')
			for index, agent in enumerate(agents):
				if index > 0: fileTmp.write(',')
				fileTmp.write('\'' + agent + '\'')
			fileTmp.write(']\n')
		elif line.find('namespaceAgents = [') != -1:
			fileTmp.write('namespaceAgents = [')
			for index, namespace in enumerate(namespaceAgents):
				if index > 0: fileTmp.write(',')
				fileTmp.write('\'' + namespace + '\'')
			fileTmp.write(']\n')
		elif line.find('world = ') != -1:
			fileTmp.write('world = \'' + world + '\'\n')
		elif line.find('srcFiles = Split(') != -1:
			fileTmp.write('srcFiles = Split(\'')
			for index, srcFile in enumerate(srcFiles):
				if index > 0: fileTmp.write(' ')
				fileTmp.write(srcFile)
			fileTmp.write('\')\n')
		else:
			fileTmp.write(line)

	file.close()
	fileTmp.close()
	os.rename(fileTmpName, fileName)

	print 'Done!'
	print '\n'

	return None

def main(modelName):
	os.chdir(pandoraPath + '/../examples/' + modelName + '/')

	agents, namespaceAgents, world, srcFiles = [], [], [], []

	readParameters(agents, namespaceAgents, world, srcFiles)
	generateMpiAndCheckpointingCode(agents, namespaceAgents)
	generateSConstruct(agents, namespaceAgents, world[0], srcFiles)

	return None

if __name__ == '__main__':
	main(sys.argv[1])