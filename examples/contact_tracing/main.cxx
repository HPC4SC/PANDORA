#include <Supermarket.hxx>
#include <SupermarketConfig.hxx>

#include <Exception.hxx>
#include <Config.hxx>
#include <RNGNormal.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) 
{
	try 
	{	
		if(argc>2) {
			throw Engine::Exception("USAGE: contact_tracing [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc!=1) 
		{
			fileName = argv[1];
		}
		Examples::Supermarket world(new Examples::SupermarketConfig(fileName), world.useOpenMPIMultiNode());
	
		world.initialize(argc, argv);
		world.setParallelism(false, false);
		world.run();
	}
	catch( std::exception & exceptionThrown ) 
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}