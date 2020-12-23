# Contact tracing using Covid tracing Apps

This model is composed by various scenarios representing the most common activities among Spanish citizens (citar estudi Carla).

The base model all of the scenarios is an epidemiological model. This base model includes the definition of the base Agent class HumanBeeing. Each step of the simulation check their surroundings updating if they are in contact with other Agents. This checking is done imitating how the Apps work and counting how many encounters with other Agents really happend and how many are registered by the Apps, using the Phone class.

The Agents algo execute an InfectAction each step, simulating Covid spread, this is done to evaluate the false positives the App generate.

All of the scenarios share the epidemiological and App aspects of the model, this is common on all of the scenario definitions. The common input parameters are the following ones:

* **numSteps**: Number of the steps of the simulation. Each step of the simulation represents one second.
* **width**: Width of the simulation space, each unit of width equals 0.5 meters.
* **height**: Height of the simulation space, each unit of height equals 0.5 meters.
* **mapRoute**: Relative route to the map representing the simulation space layout.
* **infectoinRadius**: Maximum distance that a sick Agent can infect another Agent.
* **infectiousness**: Probability of an Agent to get infected by a sick Agent if they are within the **infectionRadius**. 
* **sickRate**: Percentage of the Agent population that are sick.
* **encounterRadius**: Distance between Agents that we consider an encounter.
* **signalRadius**: Distance that the signal of the mobile Phones of the Agents.
* **applicationRate**: Percentage of the population that have the App installed.
* **phoneThreshold1**: Bluethooth lower signal value.
* **phoneThreshold2**: Bluethooth higher signal value.

The Scenarios are the following ones:

0. [Supermarket](documentation/0_SupermarketScenario.md)
1. [Street](documentation/1_StreetScenario.md)
2. [Train](documentation/2_TrainScenario.md)
3. [Train Platform](documentation/3_TrainPlatformScenario.md)
4. [Gym](documentation/4_GymScenario.md)
5. [Restaurant](documentation/5_RestaurantScenario.md)

To extract the results and analyze the movement of teh Agents on each scenario the results/visualization.py script has been implemented, specifing the route to PANDORA's output file and the number of the scenario as input a CSV file will be generated for the simulation run. (0 => Supermarket, 1 => Street, 2 => Train, 3 => Train Platform, 4 => Gym and 5=> Restaurant)