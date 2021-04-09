import sys
import os
import re

def main():
	if len(sys.argv) < 3:
		print('ERROR!')
		print('Usage: python modelName configFileName experimentID')
		print('Example: python IoTwins config-100A-1K.xml 14926369')
		exit()

	modelName = sys.argv[1]
	configFileName = sys.argv[2]
	experimentID = sys.argv[3]

	print("Python updating config file (modelName, configFileName, experimentID):")
	print(modelName)
	print(configFileName)
	print(experimentID)

	numberOfAgents = (re.findall(r'-\d+A-', configFileName)[0])[1:-2]

	homeVariable = os.path.expanduser('~')    # Get $HOME variable

	configFilePath = homeVariable + '/PANDORA/examples/' + modelName + '/configFiles/config.xml'
	configFilePathFinal = homeVariable + '/PANDORA/examples/' + modelName + '/configFiles/' + configFileName

	configFile = open(configFilePath, 'r')
	configFileFinal = open(configFilePathFinal, 'w')

	for line in configFile:
		if line.find('resultsDir=') != -1:
			line = line.replace('resultsDir="./data/"', 'resultsDir="./data/' + experimentID + '/"')
			line = line.replace('logsDir="./logs/"', 'logsDir="./logs/' + experimentID + '/"')

			#    <output resultsDir="./data/" resultsFile="IoTwins.h5" logsDir="./logs/"/>
			#    <output resultsDir="./data/14926369/" resultsFile="IoTwins.h5" logsDir="./logs/14926369/"/>

		elif line.find('numAgents=') != -1:
			line = re.sub(r'numAgents="\d+"', 'numAgents="' + numberOfAgents + '"', line)

			#    <inputData numAgents="100" numCounters="3"/>
			#    <inputData numAgents="8000" numCounters="3"/>

		configFileFinal.write(line)


	configFile.close()
	configFileFinal.close()

if __name__ == '__main__':
	main()