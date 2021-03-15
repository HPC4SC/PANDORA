library(h5)

agents=h5file("agents-0.abm")
raster=h5file("stupidModel.h5")
allstep=paste0("step",1:50)

#agents movent-se
for(i in 1:length(allstep)) {
	plot(agents["Bug"][allstep[i]]["x"][] ~ agents["Bug"][allstep[i]]["y"][], col=agents["Bug"][allstep[i]]["color"][], pch=19, xlim=c(0,100), ylim=c(0,100))
	agents=h5file("agents-0.abm")
	Sys.sleep(0.8)
}

#agent._size
for(i in 1:length(allstep)) {
	barplot(agents["Bug"][allstep[i]]["size"][], xlim=c(0,100), ylim=c(0,1000))
}

#histograma
for(i in 1:length(allstep)) {
	hist(agents["Bug"][allstep[1]]["size"][], col="green", border="red", xlim=c(0,5), ylim=c(0,100))
}

#mirat del Git del Simon, bucles funcionals
library(h5)

agents=h5file("agents-0.abm")
raster=h5file("stupidModel.h5")
allst=paste("step",1:100,sep="")
st="step1"
it=0
it=formatC(1:100,width=3,flag=0)
names(it)=allst

#moviment dels agents
sapply(allst,function(st){
	image(0:100,0:100,raster["food"][st][],col="black",axes=F,ylab="",xlab="")
	points(agents["Bug"][st]["x"][] ~ agents["Bug"][st]["y"][], col=agents["Bug"][st]["color"][], pch=19, xlim=c(0,100), ylim=c(0,100))
	Sys.sleep(0.5)
})

#creixemnet del food

sapply(allst,function(st){
	plot(1:length(raster)["food"][st][],col="green",ylab="",xlab="",pch=20)
	Sys.sleep(0.5)
})

