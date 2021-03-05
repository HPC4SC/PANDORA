import os, sys
import subprocess

pandoraPath = os.getenv('PANDORAPATH')
sys.path.append(pandoraPath + '/bin')

import generateMpi

def main():
	with open("compilingParameters.txt") as file:
		line = file.readline()

		agents, namespaceAgents, world, srcFiles, env = [], [], "", [], {}
		while line:
			keyAndValue = line.split(" = ")

			if len(keyAndValue) == 2:
				key = keyAndValue[0]
				value = keyAndValue[1]

				if key == "agents":
					agents = value.replace(" ", "").replace("\n", "").split(",")
				elif key == "namespaceAgents":
					namespaceAgents = value.replace(" ", "").replace("\n", "").split(",")
				elif key == "world":
					world = value.replace(" ", "").replace("\n", "")
				elif key == "srcFiles":
					srcFiles = value.replace(" ", "").replace("\n", "").split(",")

			line = file.readline()

		mpiAgentsSrc = ['mpiCode/FactoryCode.cxx']
		agentsSrc = ['main.cxx']
		for agent in agents:    
		    if agent != '':
		        agentsSrc.append(agent + ".cxx")
		        mpiAgentsSrc.append("mpiCode/" + agent + "_mpi.cxx")
		env["namespaces"] = namespaceAgents

		generateMpi.execute(mpiAgentsSrc, agentsSrc, env)

	return None

if __name__ == "__main__":
    main()