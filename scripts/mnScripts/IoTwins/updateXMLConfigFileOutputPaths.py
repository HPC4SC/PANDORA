import sys
import os
import re

def main():
	if len(sys.argv) < 3:
		print('ERROR!')
		print('Usage: python ' + sys.argv[0] + ' modelName configFileName experimentID')
		print('Example: python ' + sys.argv[0] + ' IoTwins config-100A-500S-1K.xml 14926369')
		exit()

	modelName = sys.argv[1]
	configFileName = sys.argv[2]
	experimentID = sys.argv[3]

	print("Python updating config file (modelName, configFileName, experimentID):")
	print(modelName)
	print(configFileName)
	print(experimentID)

	numberOfAgents = (re.findall(r'-\d+A-', configFileName)[0])[1:-2]
	numberOfSteps = (re.findall(r'-\d+S-', configFileName)[0])[1:-2]

	deployPath = os.getenv('DEPLOYMENT_PATH')
	#deployPath = os.path.expanduser('~')    # Get $HOME variable

	configFilePath = deployPath + '/PANDORA/examples/' + modelName + '/configFiles/config.xml'
	configFilePathFinal = deployPath + '/PANDORA/examples/' + modelName + '/configFiles/' + configFileName

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

		elif line.find('numSteps value=') != -1:
			line = re.sub(r'numSteps value="\d+"', 'numSteps value="' + numberOfSteps + '"', line)

		configFileFinal.write(line)


	configFile.close()
	configFileFinal.close()

if __name__ == '__main__':
	main()