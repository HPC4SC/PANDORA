# Street Scenario

The Street scenario represents one generic Street. The Walkers can go up or down the simulation space and exits trough any of it's exterior bounds. The Walkers mantain one general direction, up or down the alley, trough all of they time in the simulation.

The specific input values for this scenario are the following ones:

* **walkerRate**: Number of steps between Walkers entering the simulation.
* **drifting**: Adjuster of walking speed for better characterization.
* **stopping**: Probability to stop and not move on a signle step.
* **stopTime**: Number of seconds that the stop lasts.
* **wander**: Probability to move left or right instead of in a straight line.
* **speed**: Speed at what the Agents can move.
* **initialDensity**: Initial number of Walkers randomly placed in the simulation.