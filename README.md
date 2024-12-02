# Code-Couture
grid is the placements of each players ships
smokeGrid is the grid we rely on for radar that hides the plcaements of the ships when a smoke move is used
shipGrid has the placements of each players ship but instead of an 'X' we have the character corresponding to the number of cells of this types of ship, everytime we get a hit, we decrement according to this character to know when this specific ship is sunk
probability grid is the grid that contains the probability of each unknown cell to contain a part of the ship, we use this grid in hard mode
prevHit is the last coordinates of the hit we got 
adjacencyCounter helps us make sure that all four csurrounding cells of the prevHit are fired at
nextHit is an array containing the coordinates that we oreviosuly got a hit on and want to check its corresponding surrounding cells
isRandom when true, we use the randomFire method (since we would have gotten a miss before) and when false we use the fireAdjacency method in order to fire the surrounding cells
saveMisses is a grid that keeps track of all the cells that we fired at but got a miss in for both medium and hard modes
radarCoordinates is an array of the coordinates of the top left cell of the 2x2 area that we got an "Enemy ships found" from in order to fire at them using randomFire until we get a hit which is when we start using fireAdjacency method
radarFireCount helps us keep track of which cell we need to fire at next in the 2x2 area that we found ships in by the radar method
radarHitFound is a boolean that is true when we find the hit by using randomFire in the 2x2 area that we found ships in by the radar method
adjacencyRadar is used in order to know when we finish using the fireAdjacency method on the coordinate that we got a hit on in order to move on to another radar top-left coordinate where ships were found and repeat the same process 
calculateProbabilitu function is used to calculate the probability of each cell being a part of a ship according to the moved that were already plyaed (this method is used starting from the 4th turn in the hard mode and is called by the randomFire method)
findMaxProbability returns the coordinates of the cell that has the highest pribability of being a part of a ship in order to fire at it in the hard mode
the checkeredGrids specific to each cell helps us target the last ship in the medium and hard modes according to a checkered-like pattern that contains all the coordinates at the specific cells, that surely if fired at them all, will end up firing at this specific ship
nextCoordinates1&2 help us get the coordinates of a 2x2 area when needed, having the top-left coordinate
checksLastShipGrid initializes the checkered grid based on the last ship that the bot still has to sink
placeShips2 places the ships in a random manner for the bot
getIndex returns the index of the first available slot in which we can add another hit
getNextHit returns the index of the hit that we have to check the adjacent cells of by the fireAdjacency method
