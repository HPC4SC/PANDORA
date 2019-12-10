#include <EspaiBarca.hxx>
#include <EspaiConfig.hxx>
#include <Exception.hxx>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        if(argc>2) {
            throw Engine::Exception("USAGE: espaiBarca [config file]");
        }

        std::string fileName("config.xml");
        if(argc!=1)
        {
            fileName = argv[1];
        }
        Examples::EspaiBarca world(new Examples::EspaiConfig(fileName), world.useOpenMPSingleNode());

        world.initialize(argc, argv);
        world.run();
    }
    catch( std::exception & exceptionThrown )
    {
        std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
    }
    return 0;
}