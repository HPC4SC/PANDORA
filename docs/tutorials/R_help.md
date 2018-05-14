This tutorial intends to be a brief introduction to help you analize the
resuls of the executions of the models with R.

* Installing R

* If you din't have R installed in your computer you can install it using
the following command:
	
		```bash
		$ sudo apt-get install r-base
		$ sudo apt-get install r-base-dev
		```
		
* randomWalkers

We'll use this example allready in Pandora to introduce you how to plot 
the simulation's execution in a grid

1. First of all you have to merge the two folder /randomWalkers in the 
directory where Pandora is deployed (most likely ~/PANDORA/examples) and
we have our examples

2. Then we have to execute the simmulation using the following command:
	
	```bash
	$ ./randomWalkers
	```
	
After the execuion is done two new folders should have been generated in
/randomWalkers /logs and /data. We have to go to the /data flder in order
to analize Pandora's output.

3. Analize output

* First of all we have to go to the data folder and execute R:

	```bash
	$ cd /data
	$ R
	```
	
* Then R will execute in our terminal.

* Analize output (h5 file)
	a. We have to install the h5 library and load it into R:
	
	```bash
	$ install.packages("h5")
	$ library(h5)
	```
	
	b. Then we load the files into R variables:
	
	```bash
	agents=h5file("agents-0.abm")
	raster=h5file("RandomWalkers.h5")
	```
	
* agents contains all of randomWalkers agents, you can check a variable just typing the name of it, like this:
	
		```bash
		agents
		```
	
	c. Now we copy all of the steps of the execution into a vector (using paste0):
	
	```bash
	allstep=paste0("step",1:10)
	```
	
	d. Finally we plot all the steps using this for loop:
	
	```bash
	for ( i in 1:length(allstep)){
		plot(agents["RandomAgent"][allstep[i]]["x"][] ~ agents["RandomAgent"][allstep[i]]["y"][], col="brown", pch=19, xlim=c(0,32), ylim=c(0,32))
		Sys.sleep(.05)
	}
	```
	
