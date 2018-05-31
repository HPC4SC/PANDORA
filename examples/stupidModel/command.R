library(h5)

agents=h5file("agents-0.abm")
raster=h5file("stupidModel.h5")
allstep=paste0("step",1:100)

#agents movent-se
for(i in 1:length(allstep)){
	plot(agents["Bug"][allstep[i]]["x"][] ~ agents["Bug"][allstep[i]]["y"][], col=agents["Bug"][allstep[i]]["color"][], pch=19, xlim=c(0,100), ylim=c(0,100))
	Sys.sleep(0.2)
}

#agent._size
for(i in 1:length(allstep)){
	barplot(agents["Bug"][allstep[i]]["size"][], xlim=c(0,100), ylim=c(0,1000))
}

#histograma
for(i in 1:length(allstep)){
	hist(agents["Bug"][allstep[10]]["size"][], col="green", border="red", xlim=c(0,5), ylim=c(0,100))
}
