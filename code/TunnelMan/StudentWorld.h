#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include <string>
#include <vector>
#include <stack>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Earth;
class TunnelMan;
class Actor;
class Protester;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	~StudentWorld();

	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void updateDisplayText();

	void dig(); // Digs earth if TunnelMan is located in active earth.
	void clearBoulderEarth(const int& x, const int& y); // Clears the space a boulder occupies when initializing boulder. 
	void addActor(Actor* actor); // adds actor pointer to actors vector
	bool canMove(const int& x, const int& y, const GraphObject::Direction& d, const bool& checkEarth); 
															  // checks if a moving actor can move a spot in direction d from (x,y).
															  // checkEarth==true means we take into account earth objects.
															  // This way, we use the same function for tunnelMan and protesters.
															  // Assumes object at (x,y) is already at a valid location.
	void useSonar(const int& x, const int& y); // When tunnelMan uses sonar, make all objects within 12 visible.
	bool annoyActors(const int& x, const int& y, const int& annoyAmount, const bool& usingSquirt);
																	   // if a boulder or squirt is at (x,y), annoy all actors in their 4x4 region. 
																	   // if an annoyance occurs, return true. else return false. 
	bool bribeActors(const int& x, const int& y);
	void annoyPlayer(const int& annoyAmount); // annoy tunnelman by annoyamount
	bool withinPlayer(const int& x, const int& y, const int& distance); // checks if object at (x,y) is within distance from tunnelman
	void decBarrels(); // decrease numBarrels by 1.
	void incGold(); // tells tunnelman to increase number of gold he has
	void incSonars(); // tells tunnelman to increase number of sonars
	void incSquirts(); // tells tunnelman to increase number of squirts
	void decNumProtesters(); // decrease num protesters by 1
	void addProtester(); // method to easily add a protester. Deals with probability of hardcore, location.
	void addRandomGoodie(); // method to call to (attempt) to add water or sonar. If lucky, it will add.
	bool spotHasEarth(const int& x, const int& y); // true if 4x4 spot, starting with lower-left at (x,y), has any active earth.
	bool isFacingPlayer(const int& x, const int& y, const GraphObject::Direction& d); // checks if a protester at (x,y), facing d, is in contact with the player.
	bool inLineWithPlayer(const int& x, const int& y, GraphObject::Direction& d); // true if protester is in direct line of sight with tunnelman.
																				  // if true, Stores direction of player in d.
	std::stack<GraphObject::Direction> solveMazeToPlayer(Protester* prot); // Utilizes Protester::solveMaze() to get path to player from (x,y)


private:
	//std::vector<std::vector<Earth*>> earth; 
	Earth* earth[64][64]; // earth[x][y] is column x, row y. x=0, y=0 is bottom-left corner.
	TunnelMan* player;
	std::vector<Actor*> actors;

	int numBoulders; // number of boulders in each level
	int numGold; // number of gold nuggets in each level
	int numBarrels; // number of oil barrels in each level
	int numProtesters; // Current number of protesters active in game
	int protesterTicks; // number of ticks to pass before adding new protester
	int targetProtesters; // target number of protesters to have in level
	int probOfHardcore; // probability of protester being hardcore
	int chanceOfGoodie; // 1 in chanceOfGoodie chance that a new WaterPool or Sonar is added

};

#endif // STUDENTWORLD_H_