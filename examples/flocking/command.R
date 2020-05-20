library(h5)

agents=h5file("agents-0.abm")
raster=h5file("Flocking.h5")
allstep=paste0("step",1:1000)

for(i in 1:length(allstep)){
	plot(agents["Bird"][allstep[i]]["x"][] ~ agents["Bird"][allstep[i]]["y"][], col="yellow", pch=19, xlim=c(0,70), ylim=c(0,70))
	Sys.sleep(0.1)
}
