#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <stack>

class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, 
		Direction startDirection, float size, 
		unsigned int depth, StudentWorld* sw);

	~Actor();

	enum Blockable { notBlocking, walkBlocking, squirtBlocking };
	// notBlocking used for goodies and actors that don't block movement.
	// walkBlocking used for earth, boulder to show objects can't walk pas them. 
	// squirtBlocking used for protestors (and implicitly earth, boulder too),
	// used to determine which objects stop a squirt from moving (includes walkBlocking actors). 

	virtual void doSomething() = 0;
	virtual void getAnnoyed(int valToAnnoy); // method to annoy an actor. NOT an accessor. 
	virtual void getBribed(); // method to be bribed. NOT an accessor. 
	void move(const Direction& dir); // moves an actor one block in direction dir via moveTo(x,y)

	bool checkActive() const; // Checks whether something is alive or dead
	void setActive(const bool& status);

	bool within(const int& x1, const int& y1, const int& x2, const int& y2, const int& distance); // checks if points are within distance, inclusive.

	Blockable getBlockable() const;
	void setBlockable(const Blockable& b);

	bool getAnnoyable() const;
	void setAnnoyable(const bool& a);

	bool getBribeable() const;
	void setBribeable(const bool& b);

	StudentWorld* getWorld() const;	

private:
	StudentWorld* world; // Points to the StudentWorld we're playing in.
	bool active; // Indicates whether or not item is active in our game.
	Blockable blockable; // Used to describe if an actor blocks nothing, blocks walking, or blocks squirt
	bool annoyable; // true if actor is annoyable, else false.
	bool bribeable; // true if actor can be bribed, else false.
};

class Earth : public Actor {
public:
	Earth(int startX, int startY, StudentWorld* sw); // Rest of parameters from Actor are 
													 // predetermined for Earth Objects
	~Earth();
	virtual void doSomething();
private:
};

class TunnelMan : public Actor {
public:
	TunnelMan(StudentWorld* sw);
	~TunnelMan();

	int getHealth() const;
	void setHealth(const int& h);

	int getSquirts() const;
	void setSquirts(const int& s);

	int getSonars() const;
	void setSonars(const int& s);

	int getGold() const;
	void setGold(const int& g);

	virtual void doSomething();

	virtual void getAnnoyed(int valToAnnoy);

private:
	int hitPoints;
	int squirts;
	int sonars;
	int gold;
};

class Boulder : public Actor {
public:
	Boulder(int startX, int startY, StudentWorld* sw);
	~Boulder();
	virtual void doSomething();
private:
	enum State {stable, waiting, falling};
	State state; // determines the state of the boulder
	int waitingTicks; // keeps track of ticks that pass by while in waiting state.

};

class Squirt : public Actor {
public:
	Squirt(int startX, int startY, Direction startDirection, StudentWorld* sw);
	~Squirt();
	virtual void doSomething();
private:
	int travelDistance; // keeps track of how far it has moved
};

class Goodie : public Actor {
public:
	Goodie(int imageID, int startX, int startY,
		Direction startDirection, float size,
		unsigned int depth, StudentWorld* sw);
	~Goodie();

	virtual void doSomething();

	bool getVisibility() const;
	void setVisibility(const bool& b);

	bool getPlayerPickupable() const;
	void setPlayerPickupable(const bool& p);

	bool getPermanent() const;
	void setPermanent(const bool& p);

	int getLifetime() const;
	void setLifetime(const int& l);

private:
	bool visibility; // True means goodie is visible, false is invisible.
	bool playerPickupable; // True means player can pick up, false means protesters can pick up.
	bool permanent; // true if goodie is permanent, false if temporary.
	int lifetime; // lifetime for temporary goodies
};

class Barrel : public Goodie {
public:
	Barrel(int startX, int startY, StudentWorld* sw);
	~Barrel();
	virtual void doSomething();
private:
};

class GoldNugget : public Goodie {
public:
	GoldNugget(int startX, int startY, StudentWorld* sw, 
		bool visible, bool playerPickupable, bool permanent);
	~GoldNugget();
	virtual void doSomething();
private:
};

class SonarKit : public Goodie {
public:
	SonarKit(int startX, int startY, StudentWorld* sw);
	~SonarKit();
	virtual void doSomething();
private:
};

class WaterPool : public Goodie {
public:
	WaterPool(int startX, int startY, StudentWorld* sw);
	~WaterPool();
	virtual void doSomething();
private:

};

class Protester : public Actor {
public:
	Protester(int imageID, StudentWorld* sw);
	~Protester();

	bool getLeaveField() const;
	void setLeaveField(const bool& set);

	int getHealth() const;
	void setHealth(const int& h);

	int getTicksToWait() const;
	void setTicksToWait(const int& num);

	int getYellTicks() const;
	void setYellTicks(const int& y);

	int getNumSquaresToMove() const;
	void setNumSquaresToMove(const int& s);

	int getPerpTurnTicks() const;
	void setPerpTurnTicks(const int& p);

	bool isPerpendicular(const Direction& d1, const Direction& d2); // true if d1 and d2 are perp, false if parallel. 
	Direction getRandomPerp(const Direction& d); // gives a random direction perpendicular to d

	bool getAnnoyedHelper(const int& valToAnnoy); // returns true if we continue code after this method ends. if false, we return.
										   // This is common part of getAnnoyed() for reg and hardcore protester

	std::stack<Direction> solveMaze(const int& endX, const int& endY); // returns a stack with optimal solution from (getX(), getY()) to (endX, endY).
																	   // stack.top() gives direction actor should move to reach exit point

	bool doSomethingPart1(); // Runs steps 1-4 of protester's do something
							 // If return false, don't run part2. if true, run part2.
	bool doSomethingPart2(); // Runs steps 5-9 for reg protester, or 6-10 for hardcore.

private:
	int numSquaresToMoveInCurrentDirection; 
	int hitPoints;
	bool leaveField; // true if leaving field, else false.
	int numTicksToWait; // num ticks of rest time
	int yellTicks; // records ticks for yelling. if <= 0, can yell.
	int perpTurnTicks; // records num ticks since last perp turn
};

class RegularProtester : public Protester {
public:
	RegularProtester(StudentWorld* sw);
	~RegularProtester();

	virtual void doSomething();
	virtual void getAnnoyed(int valToAnnoy);
	virtual void getBribed();

private:
};

class HardcoreProtester : public Protester {
public:
	HardcoreProtester(StudentWorld* sw);
	~HardcoreProtester();

	virtual void doSomething();
	virtual void getAnnoyed(int valToAnnoy);
	virtual void getBribed();

private:
};

/* DIFFERENCES IN PROTESTERS:
* Regular has 5 hitPoints, hardcore has 20.
* Steps 1-4 are the same.
* Hardcore step 5 is new.
* Steps 5-9 of regular are same as steps 6-10 for hardcore.
*
* Annoyance step 5 is different. Squirting reg protester gives 100 points, hardcore gives 250
*
* Bribery step 2 is different. reg protester gives 25 points, hardcore gives 50
* steps3+ are different (the outcome of picking up a gold)
*
*/

#endif // ACTOR_H_