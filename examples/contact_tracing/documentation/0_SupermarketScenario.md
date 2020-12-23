# Supermarket Scenario

In the Supermarket Scenario there are two type of Agents, Clients and Cashiers. The Clients enter the store and after gathering the necessary products they go to the cashier area and after paying they leave the store. The Cashiers stay on the cashier area and interact with the incoming Customers. On this secanrio the Passangers move using the Shortest Path algorithm.

The specific input values for this scenario are the following ones:

* **clientRate**: Number of steps between Clients entering the simulation.
* **drifting**: Adjuster of walking speed for better characterization.
* **stopping**: Probability to stop and not move on a signle step.
* **stopTime**: Number of seconds that the stop lasts.
* **wander**: Probability to not follow the optimal path.
* **numCashiers**: Number of Cashiers on the supermarket.
* **sickCashiers**: Number of sick Cashiers on the simulation.
* **cashierShift**: Number of steps of the Cashier working shift.