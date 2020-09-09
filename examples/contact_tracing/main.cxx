#include <Supermarket.hxx>
#include <SupermarketConfig.hxx>
#include <Street.hxx>
#include <StreetConfig.hxx>

#include <Exception.hxx>
#include <Config.hxx>
#include <RNGNormal.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) 
{
	try 
	{	
		if(argc>3) {
			throw Engine::Exception("USAGE: contact_tracing [config file] scenarioType");
		}		
	
		std::string fileName("config.xml");
		if(argc!=1) 
		{
			fileName = argv[1];
		}
		Examples::Supermarket world(new Examples::SupermarketConfig(fileName), world.useOpenMPIMultiNode());
		if (*argv[2] == 0) Examples::Supermarket world(new Examples::SupermarketConfig(fileName), world.useOpenMPIMultiNode());
		else if (*argv[2] == 1) Examples::Street world(new Examples::StreetConfig(fileName), world.useOpenMPIMultiNode());
		//else if (argc[2] == 2) Examples::Train world(new Examples::SupermarketConfig(fileName), world.useOpenMPIMultiNode());
	
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