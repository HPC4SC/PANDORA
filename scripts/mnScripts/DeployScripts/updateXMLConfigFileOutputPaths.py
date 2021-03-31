import sys
import os

def main():
	if len(sys.argv) < 2:
		print('ERROR!\n')
		print('Usage: python3 configFilePath experimentID')
		print('Example: python ./configFiles/config-100A.xml 14926369')
		exit()

	configFilePath = sys.argv[1]
	experimentID = sys.argv[2]

	homeVariable = os.path.expanduser('~')    # Get $HOME variable

	configFilePath = homeVariable + '/PANDORA/' + configFilePath
	configFilePathTmp = homeVariable + '/PANDORA/' + configFilePath + '_tmp.xml'

	configFile = open(configFilePath, 'r')
	configFileTmp = open(configFilePathTmp, 'w')

	for line in configFile:
		if line.find('resultsBaseDir=') != -1:
			# Modify this line to add something like:
			#    <output resultsBaseDir="./data/" resultsFile="IoTwins.h5" logsDir="./logs/"/>
			#    <output resultsBaseDir="./data/14926369/" resultsFile="IoTwins.h5" logsDir="./logs/14926369/"/>







			

		else:
			configFileTmp.write(line)


	configFile.close()
	configFileTmp.close()

if __name__ == '__main__':
	main()