#include <Supermarket.hxx>
#include <SupermarketConfig.hxx>
#include <Street.hxx>
#include <StreetConfig.hxx>
#include <Train.hxx>
#include <TrainConfig.hxx>
#include <Gym.hxx>
#include <GymConfig.hxx>
#include <Restaurant.hxx>
#include <RestaurantConfig.hxx>

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
			throw Engine::Exception("USAGE: contact_tracing [#cores] [config file]");
		}
	
		std::string fileName("configRestaurant.xml");
		if(argc!=2) // cambiar para mpi
		{
			fileName = "configRestaurant.xml";
		}
		if (fileName == "configSupermarket.xml") {
			Examples::Supermarket super(new Examples::SupermarketConfig(fileName), super.useOpenMPIMultiNode());
			super.initialize(argc, argv);
			super.setParallelism(true, false);
			super.run();
		}
		else if (fileName == "configStreet.xml") {
			Examples::Street street(new Examples::StreetConfig(fileName), street.useOpenMPIMultiNode());
			street.initialize(argc, argv);
			street.setParallelism(true, false);
			street.run();
		}
		else if (fileName == "configTrain.xml" or fileName == "configTrainPlatform.xml") {
			Examples::Train train(new Examples::TrainConfig(fileName), train.useOpenMPIMultiNode());
			train.initialize(argc, argv);
			train.setParallelism(true, false);
			train.run();
		}
		else if (fileName == "configGym.xml") {
			Examples::Gym gym(new Examples::GymConfig(fileName), gym.useOpenMPIMultiNode());
			gym.initialize(argc, argv);
			gym.setParallelism(true, false);
			gym.run();
		}
		else if (fileName == "configRestaurant.xml") {
			Examples::Restaurant restaurant(new Examples::RestaurantConfig(fileName), restaurant.useOpenMPIMultiNode());
			restaurant.initialize(argc, argv);
			restaurant.setParallelism(true, false);
			restaurant.run();
		}
		else throw Engine::Exception("Not a known scenario");
	}
	catch( std::exception & exceptionThrown ) 
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}