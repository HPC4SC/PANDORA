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
		if (*argv[2] == '0') {
			Examples::Supermarket super(new Examples::SupermarketConfig(fileName), super.useOpenMPIMultiNode());
			super.initialize(argc, argv);
			super.setParallelism(true, false);
			super.run();
		}
		else if (*argv[2] == '1') {
			std::cout << "Going to execute street scenario" << std::endl;
			Examples::Street street(new Examples::StreetConfig(fileName), street.useOpenMPIMultiNode());
			street.initialize(argc, argv);
			street.setParallelism(true, false);
			street.run();
		}
		/*else if (argc[2] == '2') {
			Examples::Train train(new Examples::SupermarketConfig(fileName), train.useOpenMPIMultiNode());
			train.initialize(argc, argv);
			train.setParallelism(true, false);
			train.run();
		}*/
	}
	catch( std::exception & exceptionThrown ) 
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}