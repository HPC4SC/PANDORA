import sys
import os

import pandas
import numpy
import seaborn
import matplotlib.pyplot as plt

def countFileLines(filePathStr):
	with open(filePathStr) as file:
		for index, line in enumerate(file):
			pass
		return index + 1

def readFileAndParseIt(filePathStr):
	resultingDataFrame = pandas.DataFrame(columns = ("Step", "SubOverlapID", "KindOfEvent", "SubOverlapGroup", "TotalTime"), index = range(1, countFileLines(filePathStr)))
	dataFrameCounter = 1

	step_FindStr = "STEP: "
	executeAgents_FindStr = "executeAgentsInSubOverlap()"
	sendAgents_FindStr = "sendGhostAgentsInMap()"
	recvAgents_FindStr = "receiveGhostAgentsFromNeighbouringNodes()"
	sendRasters_FindStr = "sendRasterValuesInMap()"
	recvRasters_FindStr = "receiveRasters()"
	subOverlap_FindStr = "OVERLAP: "
	totalTime_FindStr = "TOTAL TIME: "

	with open(filePathStr) as file:
		line = file.readline()

		currentStep = -1
		subOverlapID, kindOfEvent, subOverlapGroup, totalTime = -1, "", "", -1.0
		while line:
			if subOverlap_FindStr in line:
				initialIndex = line.find(subOverlap_FindStr) + len(subOverlap_FindStr)
				subOverlapID = int(line[initialIndex:initialIndex + 2])
			if step_FindStr in line:
				initialIndex = line.find(step_FindStr) + len(step_FindStr)
				currentStep = int(line[initialIndex:initialIndex + 1])
			elif executeAgents_FindStr in line:
				kindOfEvent = "ExecuteAgents"
			elif sendAgents_FindStr in line:
				kindOfEvent = "SendAgents"
			elif recvAgents_FindStr in line:
				kindOfEvent = "ReceiveAgents"
			elif sendRasters_FindStr in line:
				kindOfEvent = "SendRasters"
			elif recvRasters_FindStr in line:
				kindOfEvent = "ReceiveRasters"

			if kindOfEvent != "":
				totalTime = float(line[line.find(totalTime_FindStr) + len(totalTime_FindStr):len(line) - 1])

			if totalTime != -1.0 and subOverlapID != -1:
				if subOverlapID == 0: 
					subOverlapGroup = "inside"
				elif subOverlapID in [1,2,3,4]:
					subOverlapGroup = "lateral"
				elif subOverlapID in [5,6,7,8]:
					subOverlapGroup = "corner"
				resultingDataFrame.loc[dataFrameCounter] = [currentStep, subOverlapID, kindOfEvent, subOverlapGroup, totalTime]

			dataFrameCounter = dataFrameCounter + 1
			subOverlapID, kindOfEvent, subOverlapGroup, totalTime = -1, "", "", -1.0
			line = file.readline()

	resultingDataFrame = resultingDataFrame.dropna(how = "all")
	resultingDataFrame = resultingDataFrame.astype({"Step": int, "SubOverlapID": int, "TotalTime": float})

	return resultingDataFrame

def groupDataToBePlotted(fullDataFrame):
	resultingDataFrame = fullDataFrame.groupby(["KindOfEvent", "SubOverlapGroup"], as_index = False)["TotalTime"].mean()	
	return resultingDataFrame

def plotData(dataFrame, modelName, processID, inputParamsFile, numberOfProcesses):
	seaborn.set()
	plt.figure(figsize = (18, 9))

	plot = seaborn.boxplot(x = 'KindOfEvent', y = 'TotalTime', hue = "SubOverlapGroup", data = dataFrame).set_title("ModelName: " + modelName + "_params:" + inputParamsFile + "_K:" + str(numberOfProcesses) + " - Process: " + processID)

	figure = plot.get_figure()
	figure.savefig("./Graphs/" + modelName + "/" + inputParamsFile + "/InstrumentationTimes_" + modelName + "_" + inputParamsFile + "_" + str(numberOfProcesses) + "_Process_" + processID + ".png")
	plt.clf()
	return 0

def main():
	if len(sys.argv) < 4:
		print("ERROR!\n")
		print("Usage: python3 makePerformanceGraphs.py modelName inputParamsFile numberOfProcesses")
		print("Example: python makePerformanceGraphs.py stupidModel 20x20-17 4")
		exit()

	modelName = sys.argv[1]
	inputParamsFile = sys.argv[2]
	numberOfProcesses = int(sys.argv[3])

	workingDirectory = os.getenv("HOME") + "/PANDORA/examples/" + modelName + "/logs/"

	plotsDirectory = "./Graphs/" + modelName + "/" + inputParamsFile + "/"
	if not os.path.exists(plotsDirectory): os.makedirs(plotsDirectory)

	filesToBePlotted = ["InstrumentationProcess_" + str(i) + ".log" for i in range(numberOfProcesses)]
	for fileNameStr in os.listdir(workingDirectory):

		if fileNameStr in filesToBePlotted:
			processID = fileNameStr.split("_")[1].split(".")[0]

			fullDataFrame = readFileAndParseIt(workingDirectory + fileNameStr)
			#reducedDataFrame = groupDataToBePlotted(fullDataFrame)

			plotData(fullDataFrame, modelName, processID, inputParamsFile, numberOfProcesses)

	return 0

if __name__ == "__main__":
    main()