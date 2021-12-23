#include "Actor.h"
#include "StudentWorld.h"
#include "GameController.h"
#include <queue>
#include <algorithm>


Actor::Actor(int imageID, int startX, int startY,
	Direction startDirection, float size,
	unsigned int depth, StudentWorld* sw)
	: GraphObject(imageID, startX, startY, startDirection, size, depth),
	world(sw), active(true), blockable(notBlocking), annoyable(false), bribeable(false) {

	setVisible(true);

}

Actor::~Actor() {}

void Actor::getAnnoyed(int valToAnnoy) {
	return;
}

void Actor::getBribed() {
	return;
}

void Actor::move(const Direction& dir) {
	switch (dir) {
	case right:
		setDirection(right);
		moveTo(getX() + 1, getY());
		break;
	case left:
		setDirection(left);
		moveTo(getX() - 1, getY());
		break;
	case up:
		setDirection(up);
		moveTo(getX(), getY() + 1);
		break;
	case down:
		setDirection(down);
		moveTo(getX(), getY() - 1);
		break;
	}
}

bool Actor::checkActive() const {
	return active;
}
void Actor::setActive(const bool& status) {
	active = status;
	setVisible(status);
}

bool Actor::within(const int& x1, const int& y1, const int& x2, const int& y2, const int& distance) {
	int dx = x1 - x2;
	int dy = y1 - y2;
	if ((dx * dx) + (dy * dy) <= distance * distance)
		return true;
	return false;
}

Actor::Blockable Actor::getBlockable() const {
	return blockable;
}

void Actor::setBlockable(const Blockable& b) {
	blockable = b;
}

bool Actor::getAnnoyable() const {
	return annoyable;
}

void Actor::setAnnoyable(const bool& a) {
	annoyable = a;
}

bool Actor::getBribeable() const {
	return bribeable;
}

void Actor::setBribeable(const bool& b) {
	bribeable = b;
}

StudentWorld* Actor::getWorld() const {
	return world;
}

Earth::Earth(int startX, int startY, StudentWorld* sw)
	:Actor(TID_EARTH, startX, startY, right, 0.25, 3, sw) {
	setBlockable(walkBlocking);
}

Earth::~Earth() {}

void Earth::doSomething() {
	return; // Earth objects don't do anything
}


TunnelMan::TunnelMan(StudentWorld* sw)
	: Actor(TID_PLAYER, 30, 60, right, 1.0, 0, sw),
	hitPoints(10), squirts(5), sonars(1), gold(0) {
	setBlockable(squirtBlocking); // used to check if protester hits tunnelMan
	setAnnoyable(true);
}

TunnelMan::~TunnelMan() {}

int TunnelMan::getHealth() const {
	return hitPoints;
}

void TunnelMan::setHealth(const int& h) {
	hitPoints = h;
}

int TunnelMan::getSquirts() const {
	return squirts;
}

void TunnelMan::setSquirts(const int& s) {
	squirts = s;
}

int TunnelMan::getSonars() const {
	return sonars;
}

void TunnelMan::setSonars(const int& s) {
	sonars = s;
}

int TunnelMan::getGold() const {
	return gold;
}

void TunnelMan::setGold(const int& g) {
	gold = g;
}

void TunnelMan::doSomething() {
	if (checkActive() == false)
		return;

	int ch;
	if (getWorld()->getKey(ch) == true) {		// Player pressed a key
		switch (ch) {
		case KEY_PRESS_ESCAPE:
			setActive(false); // dead
			break;

		case KEY_PRESS_SPACE: // fire squirt
			if (squirts > 0) {
				squirts--;
				GameController::getInstance().playSound(SOUND_PLAYER_SQUIRT);
				Direction d = getDirection();
				bool squirtGun = true;

				switch (d) {
				case up:
					for (int i = 0; i < 4; i++) {
						if (!getWorld()->canMove(getX(), getY() + i, up, true)) squirtGun = false;
					}
					if (squirtGun) getWorld()->addActor(new Squirt(getX(), getY() + 3, up, getWorld()));
					break;

				case down:
					for (int i = 0; i < 4; i++) {
						if (!getWorld()->canMove(getX(), getY() - i, down, true)) squirtGun = false;
					}
					if (squirtGun) getWorld()->addActor(new Squirt(getX(), getY() - 3, down, getWorld()));
					break;

				case right:
					for (int i = 0; i < 4; i++) {
						if (!getWorld()->canMove(getX() + i, getY(), right, true)) squirtGun = false;
					}
					if (squirtGun) getWorld()->addActor(new Squirt(getX() + 3, getY(), right, getWorld()));
					break;

				case left:
					for (int i = 0; i < 4; i++) {
						if (!getWorld()->canMove(getX() - i, getY(), left, true)) squirtGun = false;
					}
					if (squirtGun) getWorld()->addActor(new Squirt(getX() - 3, getY(), left, getWorld()));
					break;
				}
			}
			break;

		case KEY_PRESS_RIGHT:
			if (getDirection() == right && getWorld()->canMove(getX(), getY(), right, false)) // If facing right with space to move, then move.
				moveTo(getX() + 1, getY());
			else
				setDirection(right);
			break;

		case KEY_PRESS_LEFT:
			if (getDirection() == left && getWorld()->canMove(getX(), getY(), left, false))
				moveTo(getX() - 1, getY());
			else
				setDirection(left);
			break;

		case KEY_PRESS_UP:
			if (getDirection() == up && getWorld()->canMove(getX(), getY(), up, false))
				moveTo(getX(), getY() + 1);
			else
				setDirection(up);
			break;

		case KEY_PRESS_DOWN:
			if (getDirection() == down && getWorld()->canMove(getX(), getY(), down, false))
				moveTo(getX(), getY() - 1);
			else
				setDirection(down);
			break;

		case 'Z':
		case 'z':
			if (sonars > 0) {
				sonars--;
				getWorld()->useSonar(getX(), getY());
			}
			break;

		case KEY_PRESS_TAB:
			if (gold > 0) {
				gold--;
				getWorld()->addActor(new GoldNugget(getX(), getY(), getWorld(), true, false, false));
			}
			break;
		}
	}

	// If there is earth where we just moved, dig it out w/sound. 
	getWorld()->dig();
}

void TunnelMan::getAnnoyed(int valToAnnoy) {
	hitPoints -= valToAnnoy;
	if (hitPoints <= 0) {
		setActive(false);
		GameController::getInstance().playSound(SOUND_PLAYER_GIVE_UP);
	}
}


Boulder::Boulder(int startX, int startY, StudentWorld* sw) 
	: Actor(TID_BOULDER, startX, startY, down, 1.0, 1, sw),
	state(stable), waitingTicks(30) {
	setBlockable(walkBlocking);
	getWorld()->clearBoulderEarth(startX, startY);
}

Boulder::~Boulder() {}

void Boulder::doSomething() {
	if (checkActive() == false)
		return;

	if (state == stable) {
		if (getWorld()->canMove(getX(), getY(), down, true)) // if there is no earth below, go into waiting.
			state = waiting;
	}

	if (state == waiting) {
		waitingTicks--;
	}

	if (state == waiting && waitingTicks == 0) {
		GameController::getInstance().playSound(SOUND_FALLING_ROCK);
		state = falling;
		setBlockable(notBlocking); // Used so squirt can pass through moving boulder, as per sample game.
	}

	if (state == falling) {
		if (getWorld()->canMove(getX(), getY(), down, true)) {
			moveTo(getX(), getY() - 1);
		}
		else {
			setActive(false);
		}
		getWorld()->annoyActors(getX(), getY(), 100, false);
	}


}

Squirt::Squirt(int startX, int startY, Direction startDirection, StudentWorld* sw)
	:Actor(TID_WATER_SPURT, startX, startY, startDirection, 1.0, 1, sw),
	travelDistance(4) {}

Squirt::~Squirt() {}

void Squirt::doSomething() {
	// TunnelMan checks whether the squirt object can be initially shown when created.
	if (travelDistance == 0) {
		setActive(false);
		return;
	}

	if (!getWorld()->canMove(getX(), getY(), getDirection(), true)) {
		setActive(false);
		return;
	}

	if (travelDistance > 0) { // MOVE
		move(getDirection());
		travelDistance--;
	}
	if (getWorld()->annoyActors(getX(), getY(), 2, true))
		travelDistance = 0; // if we annoyed an actor, set travelDistance to 0 so it can be removed.
}

Goodie::Goodie(int imageID, int startX, int startY,
	Direction startDirection, float size,
	unsigned int depth, StudentWorld* sw)
	:Actor(imageID, startX, startY, startDirection, size, depth, sw),
	lifetime(100), permanent(true), playerPickupable(true), visibility(true) {}
	// ^^ all these 4 values are random. Will be overwritten in subclasses.


Goodie::~Goodie() {}

void Goodie::doSomething() { return; }

bool Goodie::getVisibility() const {
	return visibility;
}

void Goodie::setVisibility(const bool& b) {
	visibility = b;
}

bool Goodie::getPlayerPickupable() const {
	return playerPickupable;
}

void Goodie::setPlayerPickupable(const bool& p) {
	playerPickupable = p;
}

bool Goodie::getPermanent() const {
	return permanent;
}

void Goodie::setPermanent(const bool& p) {
	permanent = p;
}

int Goodie::getLifetime() const {
	return lifetime;
}

void Goodie::setLifetime(const int& l) {
	lifetime = l;
}

Barrel::Barrel(int startX, int startY, StudentWorld* sw)
	:Goodie(TID_BARREL, startX, startY, right, 1.0, 2, sw) {
	setVisible(false);
}

Barrel::~Barrel() {}

void Barrel::doSomething() {
	if (checkActive() == false) return;

	if (getWorld()->withinPlayer(getX(), getY(), 3)) {
		setActive(false);
		GameController::getInstance().playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decBarrels();
		return;
	}

	if (getWorld()->withinPlayer(getX(), getY(), 4)) {
		setVisible(true);
		return;
	}
}

GoldNugget::GoldNugget(int startX, int startY, StudentWorld* sw,
	bool visible, bool playerPickupable, bool permanent)
	: Goodie(TID_GOLD, startX, startY, right, 1.0, 2, sw) {
		setVisible(visible);
		setVisibility(visible);
		setPlayerPickupable(playerPickupable);
		setPermanent(permanent);
		setLifetime(100);
		
}

GoldNugget::~GoldNugget() {}

void GoldNugget::doSomething() {
	if (checkActive() == false) return;

	if (getVisibility() == false && getWorld()->withinPlayer(getX(), getY(), 4)) {
		setVisible(true);
		setVisibility(true); // used for keeping track since can't use isVisible();
		return;
	}

	if (getWorld()->withinPlayer(getX(), getY(), 3) && getPlayerPickupable() == true) { // TODO: might have error here
		setActive(false);
		GameController::getInstance().playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->incGold();
		return;
	}

	if (getPlayerPickupable() == false) {
		if (getWorld()->bribeActors(getX(), getY())) {	// if we bribed an actor, this will be true
			setActive(false);
			// Sound of picking up gold occurs in getBribed(); for protesters
			// score increase occurs in protester's getBribed();
		}
	}
	if (getPermanent() == false) {
		setLifetime(getLifetime() - 1);
	}
	if (getLifetime() == 0) {
		setActive(false);
	}

}

SonarKit::SonarKit(int startX, int startY, StudentWorld* sw)
	: Goodie(TID_SONAR, startX, startY, right, 1.0, 2, sw) {
	setVisibility(true);
	setPlayerPickupable(true);
	setPermanent(false);
	setLifetime(std::max(100, 300 - (int)getWorld()->getLevel() * 10));
}

SonarKit::~SonarKit() {}

void SonarKit::doSomething() {
	if (checkActive() == false) return;

	if (getWorld()->withinPlayer(getX(), getY(), 3)) {
		setActive(false);
		GameController::getInstance().playSound(SOUND_GOT_GOODIE);
		getWorld()->incSonars();
		getWorld()->increaseScore(75);
	}

	setLifetime(getLifetime() - 1);
	if (getLifetime() <= 0) {
		setActive(false);
	}
}

WaterPool::WaterPool(int startX, int startY, StudentWorld* sw)
	: Goodie(TID_WATER_POOL, startX, startY, right, 1.0, 2, sw) {
	setVisibility(true);
	setPlayerPickupable(true);
	setPermanent(false);
	setLifetime(std::max(100, 300 - (int)getWorld()->getLevel() * 10));
}

WaterPool::~WaterPool() {}

void WaterPool::doSomething() {
	if (checkActive() == false) return;
	if (getWorld()->withinPlayer(getX(), getY(), 3)) {
		setActive(false);
		GameController::getInstance().playSound(SOUND_GOT_GOODIE);
		getWorld()->incSquirts();
		getWorld()->increaseScore(100);
	}
	setLifetime(getLifetime() - 1);
	if (getLifetime() <= 0) {
		setActive(false);
	}
}

Protester::Protester(int imageID, StudentWorld* sw)
	: Actor(imageID, 60, 60,
		left, 1.0, 0, sw), numSquaresToMoveInCurrentDirection(rand() % 53 + 8) {
	setLeaveField(false);
	setBlockable(squirtBlocking);
	setAnnoyable(true);
	setBribeable(true);
	setYellTicks(0); // hasn't shouted yet, so can now shout
	setPerpTurnTicks(0); // starts at 0. After each non-resting tick, goes up. if >200 at junction, turn perp. 

	numTicksToWait = std::max(0, 3 - (int)getWorld()->getLevel() / 4);
}

Protester::~Protester() {}

bool Protester::getLeaveField() const {
	return leaveField;
}

void Protester::setLeaveField(const bool& set) {
	leaveField = set;
}

int Protester::getHealth() const {
	return hitPoints;
}

void Protester::setHealth(const int& h) {
	hitPoints = h;
}

int Protester::getTicksToWait() const {
	return numTicksToWait;
}

void Protester::setTicksToWait(const int& num) {
	numTicksToWait = num;
}

int Protester::getYellTicks() const {
	return yellTicks;
}

void Protester::setYellTicks(const int& y) {
	yellTicks = y;
}

int Protester::getNumSquaresToMove() const {
	return numSquaresToMoveInCurrentDirection;
}

void Protester::setNumSquaresToMove(const int& s) {
	numSquaresToMoveInCurrentDirection = s;
}

int Protester::getPerpTurnTicks() const {
	return perpTurnTicks;
}

void Protester::setPerpTurnTicks(const int& p) {
	perpTurnTicks = p;
}

bool Protester::isPerpendicular(const Direction& d1, const Direction& d2) {
	switch (d1) {
	case left:
	case right:
		if (d2 == up || d2 == down)
			return true;
		else
			return false;
	case up:
	case down:
	default: // not needed or used. Only used to get rid of control path return warning C4715
		if (d2 == left || d2 == right)
			return true;
		else
			return false;
	}
}

GraphObject::Direction Protester::getRandomPerp(const Direction& d) {
	int temp = rand() % 2; // gives 0 or 1
	switch (d) {
	case left:
	case right:
		if (temp == 0) return up;
		else return down;
	case up:
	case down:
	default: // not needed or used. Only used to get rid of control path return warning C4715
		if (temp == 0) return left;
		else return right;
	}
}

std::stack<GraphObject::Direction> Protester::solveMaze(const int& endX, const int& endY) {
	bool boolField[64][64];
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			boolField[i][j] = false;
		}
	}
	std::pair<std::pair<int, int>, Direction> dirField[64][64];
	std::queue<std::pair<int, int>> Q;
	std::stack<GraphObject::Direction> S;
	Q.push(std::make_pair(getX(), getY()));
	boolField[getX()][getY()] = true;
	std::pair<int, int> cur;
	int pathLength = 0;

	// we don't check if queue is empty because we know there is a solution;
	// if a protester got to (x,y), it can simply retrace path.
	while (true) {		// this will eventually end since we know there is a solution
		cur = Q.front();
		Q.pop();
		if (cur.first == endX && cur.second == endY)
			break;
		// if space above is unvisited and we can move there, then move.
		if (getWorld()->canMove(cur.first, cur.second, up, true) && boolField[cur.first][cur.second + 1] == false) {
			boolField[cur.first][cur.second + 1] = true; //mark as visited
			Q.push(std::make_pair(cur.first, cur.second + 1));
			dirField[cur.first][cur.second + 1] = std::make_pair(std::make_pair(cur.first, cur.second), up);
		}
		if (getWorld()->canMove(cur.first, cur.second, right, true) && boolField[cur.first + 1][cur.second] == false) {
			boolField[cur.first + 1][cur.second] = true; //mark as visited
			Q.push(std::make_pair(cur.first + 1, cur.second));
			dirField[cur.first + 1][cur.second] = std::make_pair(std::make_pair(cur.first, cur.second), right);
		}
		if (getWorld()->canMove(cur.first, cur.second, down, true) && boolField[cur.first][cur.second - 1] == false) {
			boolField[cur.first][cur.second - 1] = true; //mark as visited
			Q.push(std::make_pair(cur.first, cur.second - 1));
			dirField[cur.first][cur.second - 1] = std::make_pair(std::make_pair(cur.first, cur.second), down);
		}
		if (getWorld()->canMove(cur.first, cur.second, left, true) && boolField[cur.first - 1][cur.second] == false) {
			boolField[cur.first - 1][cur.second] = true; //mark as visited
			Q.push(std::make_pair(cur.first - 1, cur.second));
			dirField[cur.first - 1][cur.second] = std::make_pair(std::make_pair(cur.first, cur.second), left);
		}
	}
	int curX = endX, curY = endY;
	while (curX!= getX() || curY != getY()) {
		S.push(dirField[curX][curY].second);
		int tempX = dirField[curX][curY].first.first;
		int tempY = dirField[curX][curY].first.second;
		curX = tempX;
		curY = tempY;
	}
	pathLength = S.size();
	return S;
}

bool Protester::getAnnoyedHelper(const int& valToAnnoy) {
	if (getLeaveField() == true) return false; // can't annoy when leaving field
	hitPoints -= valToAnnoy;
	if (hitPoints > 0) {
		GameController::getInstance().playSound(SOUND_PROTESTER_ANNOYED);
		numTicksToWait = std::max(50, 100 - (int)getWorld()->getLevel() * 10);
	}
	if (hitPoints <= 0) {		// hitPoints <= 0 , so they are dead.
		leaveField = true;
		GameController::getInstance().playSound(SOUND_PROTESTER_GIVE_UP);
		numTicksToWait = 0;
		//if (valToAnnoy == 2) getWorld()->increaseScore(100); 
		//else if (valToAnnoy == 100) getWorld()->increaseScore(500); // same logic as above for boulder
	}
	return true;
}

bool Protester::doSomethingPart1() {
	// Step 1
	if (checkActive() == false) return false;

	// Step 2
	if (getTicksToWait() > 0) {
		setTicksToWait(getTicksToWait() - 1);
		return false;
	}
	setTicksToWait(std::max(0, 3 - (int)getWorld()->getLevel() / 4));
	setYellTicks(getYellTicks() - 1);

	// Step 3
	if (getLeaveField() == true) {
		if (getX() == 60 && getY() == 60) {		// at the exit
			setActive(false);
			getWorld()->decNumProtesters(); // one protester left, so must decrease by one.
			return false;
		} 
		Direction d = solveMaze(60, 60).top();
		// MOVE
		move(d);
		return false;
	}
	// Step 4
	if (getWorld()->isFacingPlayer(getX(), getY(), getDirection())) {
		if (getYellTicks() <= 0) {
			GameController::getInstance().playSound(SOUND_PROTESTER_YELL);
			getWorld()->annoyPlayer(2);
			setYellTicks(15);
		}
		return false;
	}
	return true;
}

bool Protester::doSomethingPart2() {
	// Step 5
	Direction d = getDirection();
	Direction prevDir = getDirection();
	if (getWorld()->inLineWithPlayer(getX(), getY(), d) && !getWorld()->withinPlayer(getX(), getY(), 4)) {
		setDirection(d);
		if (isPerpendicular(d, prevDir) == true) setPerpTurnTicks(0); // we will make a perp turn, so reset count since last perp turn.
		else setPerpTurnTicks(getPerpTurnTicks() + 1); // not a perp turn, so +=1 
		// MOVE on d
		move(d);
		setNumSquaresToMove(0);
		return false;
	}

	// Step 6
	setNumSquaresToMove(getNumSquaresToMove() - 1);
	if (getNumSquaresToMove() <= 0) {
		bool flag = true;
		prevDir = getDirection();
		while (flag) {
			int temp = rand() % 4;
			switch (temp) {
			case 0: // simulates up
				if (getWorld()->canMove(getX(), getY(), up, true)) {
					setDirection(up);
					flag = false;
				}
				break;
			case 1: // simulates right
				if (getWorld()->canMove(getX(), getY(), right, true)) {
					setDirection(right);
					flag = false;
				}
				break;
			case 2: // simulates down
				if (getWorld()->canMove(getX(), getY(), down, true)) {
					setDirection(down);
					flag = false;
				}
				break;
			case 3: // simulates left
				if (getWorld()->canMove(getX(), getY(), left, true)) {
					setDirection(left);
					flag = false;
				}
			}

		}
		setNumSquaresToMove(rand() % 53 + 8);
		if (isPerpendicular(prevDir, getDirection()) == true) setPerpTurnTicks(0); // if new dir is perp to old, reset perpTicks.
		else setPerpTurnTicks(getPerpTurnTicks() + 1); // not a perp turn, so +=1 
	}

	// Step 7. check if we're at an intersection:
	else {
		bool flag = false; // set to false just to remove warning. false won't be used at all. 
		switch (getDirection()) {
		case left:
		case right:
			if (getWorld()->canMove(getX(), getY(), up, true) || getWorld()->canMove(getX(), getY(), down, true))
				flag = true;
			else flag = false;
			break;
		case up:
		case down:
			if (getWorld()->canMove(getX(), getY(), left, true) || getWorld()->canMove(getX(), getY(), right, true))
				flag = true;
			else flag = false;
			break;
		} // if we can move perp and haven't turned perp in 200+ non-resting ticks:
		if (flag && getPerpTurnTicks() >= 200) {
			while (true) {
				// pick random perp direction:
				Direction newDir = getRandomPerp(getDirection());
				if (!getWorld()->canMove(getX(), getY(), newDir, true)) {
					continue; // if we can't move in one perp direction, retry. 
							  // kep trying for other perp direction until we can move that way
				}
				setDirection(newDir); // if we reach here, then newDir is viable choice. set direction to it. 
				setPerpTurnTicks(0); // we just turned perp, so reset counter to 0.
				break; // we have a perp direction, newDir, that we can move to. Thus, leave loop.
			}
			setNumSquaresToMove(rand() % 53 + 8);
		}
	}

	// Step 8
	if (getWorld()->canMove(getX(), getY(), getDirection(), true)) {
		// MOVE
		move(getDirection());
	}

	// Step 9
	else {
		setNumSquaresToMove(0);
	}
	return true;
}

RegularProtester::RegularProtester(StudentWorld* sw)
	:Protester(TID_PROTESTER, sw) {
	setHealth(5);
}

RegularProtester::~RegularProtester() {}

void RegularProtester::doSomething() {
	if (doSomethingPart1() == false) return; // if part1 returned (false), then nothing else should occur.
	doSomethingPart2();
}

void RegularProtester::getAnnoyed(int valToAnnoy) {
	if (getAnnoyedHelper(valToAnnoy) == false) return;

	if (getHealth() <= 0) {
		if (valToAnnoy == 2) getWorld()->increaseScore(100); // squirts cause 2 damage.
														 // if protester was annoyed by 2 and killed, it must've been from squirt,
														 // thus, we conclude to give 100 points.
		else if (valToAnnoy == 100) getWorld()->increaseScore(500); // same logic as above for boulder
	}
}

void RegularProtester::getBribed() {
	GameController::getInstance().playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	setLeaveField(true);
}

HardcoreProtester::HardcoreProtester(StudentWorld* sw)
	:Protester(TID_HARD_CORE_PROTESTER, sw) {
	setHealth(20);
}

HardcoreProtester::~HardcoreProtester() {}

void HardcoreProtester::doSomething() {
	if (doSomethingPart1() == false) return;

	// Step 5 of Hardcore (different from regular)
	if (getWorld()->withinPlayer(getX(), getY(), 4) == false) { // not within 4 units
		int M = 16 + getWorld()->getLevel() * 2;
		std::stack<GraphObject::Direction> directions = getWorld()->solveMazeToPlayer(this);
		if ((int)directions.size() <= M) {
			setDirection(directions.top());
			// MOVE on directions.top() or getDirection()
			move(getDirection());
			return;
		}
	}
	doSomethingPart2();
}

void HardcoreProtester::getAnnoyed(int valToAnnoy) {
	if (getAnnoyedHelper(valToAnnoy) == false) return;

	if (getHealth() <= 0) {
		if (valToAnnoy == 2) getWorld()->increaseScore(250); // squirts cause 2 damage.
														 // if protester was annoyed by 2 and killed, it must've been from squirt,
														 // thus, we conclude to give 100 points.
		else if (valToAnnoy == 100) getWorld()->increaseScore(500); // same logic as above for boulder
	}
}

void HardcoreProtester::getBribed() {
	GameController::getInstance().playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(50);
	int ticksToStare = std::max(50, 100 - (int)getWorld()->getLevel() * 10);
	setTicksToWait(getTicksToWait() + ticksToStare); // increase resting ticks by that amount
}