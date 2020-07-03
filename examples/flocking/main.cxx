
#include <RandomWorld.hxx>
#include <RandomWorldConfig.hxx>
#include <Exception.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char *argv[]) 
{
	try 
	{
		if(argc>2) 
		{
			throw Engine::Exception("USAGE: Flocking [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc!=1) 
		{
			fileName = argv[1];
		}
		Examples::RandomWorld world(new Examples::RandomWorldConfig(fileName), world.useOpenMPIMultiNode());
	
		world.initialize(argc, argv);
		world.setParallelism(true, false);
		world.run();
	}
	catch( std::exception & exceptionThrown ) 
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

