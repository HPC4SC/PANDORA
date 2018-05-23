library(h5)

agents=h5file("agents-0.abm")
raster=h5file("Flocking.h5")
allstep=paste0("step",1:30)

for(i in 1:length(allstep)){
	plot(agents["Bird"][allstep[i]]["x"][] ~ agents["Bird"][allstep[i]]["y"][], col="yellow", pch=19, xlim=c(0,50), ylim=c(0,50))
	Sys.sleep(.05)
}
