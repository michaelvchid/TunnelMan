#include "StudentWorld.h"
#include "Actor.h"
#include "GameController.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir) {
	/*
	for (size_t x = 0; x < 64; x++) { // TODO: check whether to use 64 or some global constant
		vector<Earth*> v;
		for (size_t y = 0; y < 64; y++) {
			v.push_back(new Earth(x, y, this));
		}
		earth.push_back(v);
	}
	*/
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 64; y++) {
			earth[x][y] = new Earth(x, y, this);
		}
	}
}

StudentWorld::~StudentWorld() {
	for (size_t x = 0; x < 64; x++) {
		for (size_t y = 0; y < 64; y++) {
			delete earth[x][y];
		}
	}
}


int StudentWorld::init()
{
	player = new TunnelMan(this);
	for (size_t x = 0; x < 64; x++) {
		for (size_t y = 0; y < 64; y++) {
			if ((30 <= x && x <= 33 && y >= 4) || y >= 60) {
				earth[x][y]->setActive(false);
			}
			else {
				earth[x][y]->setActive(true);
			}
		}
	}

	numBoulders = min((int)getLevel() / 2 + 2, 9);
	numGold = max(5 - (int)getLevel() / 2, 2);
	numBarrels = min(2 + (int)getLevel(), 21);
	numProtesters = 0;
	protesterTicks = max(25, 200 - (int)getLevel());
	targetProtesters = (int) min(15.0, 2 + getLevel() * 1.5);
	probOfHardcore = min(90, (int)getLevel() * 10 + 30);
	chanceOfGoodie = getLevel() * 25 + 300;

	int counter = 0;
	while (counter < numBoulders) {
		int x = rand() % 61;
		int y = rand() % 37 + 20;
		if (26 <= x && x <= 34) // If touching the empty mineshaft, reroll (x,y).
			continue;
		bool addActor = true;
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			if ((*it)->within((*it)->getX(), (*it)->getY(), x, y, 6)) {
				addActor = false;		// if an actor is within our intended (x,y), try again with new (x,y).
			}
		}
		if (addActor) {
			actors.push_back(new Boulder(x, y, this));
			counter++;
		}
	}

	counter = 0;
	while (counter < numGold) {
		int x = rand() % 61;
		int y = rand() % 57;
		if (26 <= x && x <= 34) // If touching the empty mineshaft, reroll (x,y).
			continue;
		bool addActor = true;
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			if ((*it)->within((*it)->getX(), (*it)->getY(), x, y, 6)) {
				addActor = false;		// if an actor is within our intended (x,y), try again with new (x,y).
			}
		}
		if (addActor) {
			actors.push_back(new GoldNugget(x, y, this, false, true, true));
			counter++;
		}
	}

	counter = 0;
	while (counter < numBarrels) {
		int x = rand() % 61;
		int y = rand() % 57;
		if (26 <= x && x <= 34) // If touching the empty mineshaft, reroll (x,y).
			continue;
		bool addActor = true;
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			if ((*it)->within((*it)->getX(), (*it)->getY(), x, y, 6)) {
				addActor = false;		// if an actor is within our intended (x,y), try again with new (x,y).
			}
		}
		if (addActor) {
			actors.push_back(new Barrel(x, y, this));
			counter++;
		}
	}

	// Must always start level with one protester
	addProtester();

	// TODO: DELETE THESE ACTORS. THEYRE USED FOR TESTING PURPOSES ONLY
	/*
	HardcoreProtester* ptr = new HardcoreProtester(this);
	ptr->moveTo(30, 50);
	actors.push_back(ptr);
	actors.push_back(new Barrel(10, 50, this));
	actors.push_back(new GoldNugget(20, 50, this, false, true, true));
	actors.push_back(new SonarKit(50, 60, this));
	*/

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

	// 1. update text
	updateDisplayText();

	// 2. make actors do something
	player->doSomething();

	for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
		(*it)->doSomething();
		if (player->checkActive() == false) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (numBarrels == 0) {
			GameController::getInstance().playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}

	// 3. delete inactive actors
	vector<Actor*>::iterator it = actors.begin();
	while (it != actors.end()) {
		if ((*it)->checkActive() == false) {
			delete (*it);
			actors.erase(it);
			it = actors.begin();
		}
		else {
			it++;
		}
	}
	
	// Add actors
	//Protesters: must check if we can add a new one. If so, we add
	if (protesterTicks <= 0 && numProtesters < targetProtesters) {
		addProtester();
		protesterTicks = max(25, 200 - (int)getLevel());
	}
	else
		protesterTicks--;

	// Goodies: we don't need to check if we can add a goodie. Helper function already checks
	addRandomGoodie();

	// If player is dead, end level.
	if (player->checkActive() == false) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	// if we win, go to next level.
	if (numBarrels == 0) {
		GameController::getInstance().playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	// To remove player and create new one. Makes game run smoother
	// than if we use a single player and move(x,y).
	delete player;
	vector<Actor*>::iterator it = actors.begin();
	while (it != actors.end()) {
		delete (*it);
		actors.erase(it);
		it = actors.begin();
	}
	return;
}

void StudentWorld::updateDisplayText() {
	ostringstream oss;
	oss.fill('0');
	oss << "  Scr: " << setw(6) << getScore() << "  ";
	oss.fill(' ');
	oss << "Lvl: " << setw(2) << getLevel() << "  ";
	oss << "Lives: " << setw(1) << getLives() << "  ";
	oss << "Hlth: " << setw(3) << player->getHealth() * 10 << "%  ";
	oss << "Wtr: " << setw(2) << player->getSquirts() << "  ";
	oss << "Gld: " << setw(2) << player->getGold() << "  ";
	oss << "Sonar: " << setw(2) << player->getSonars() << "  ";
	oss << "Oil Left: " << setw(2) << numBarrels << "  ";

	string s = oss.str();
	setGameStatText(s);


}

void StudentWorld::dig() {
	bool flag = false;
	for (int x = player->getX(); x <= player->getX() + 3; x++) {
		for (int y = player->getY(); y <= player->getY() + 3; y++) {
			if (y <= 59 && earth[x][y]->checkActive()) {
				earth[x][y]->setActive(false);
				flag = true;
			}
		}
	}
	if (flag) {
		GameController::getInstance().playSound(SOUND_DIG);
	}
}

void StudentWorld::clearBoulderEarth(const int& x, const int& y) {
	for (int i = x; i <= x + 3; i++) {
		for (int j = y; j <= y + 3; j++) {
			earth[i][j]->setActive(false);
		}
	}
}

void StudentWorld::addActor(Actor* newActor) {
	actors.push_back(newActor);
}

bool StudentWorld::canMove(const int& x, const int& y, const GraphObject::Direction& d, const bool& checkEarth) {

	switch(d) {
	case Actor::right:
		if (x >= 60) return false; // Can't move right, out of bounds
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			for (int i = -3; i <= 3; i++) {
				if ((*it)->getX() == x + 4 && (*it)->getY() == y + i && (*it)->getBlockable() == Actor::walkBlocking) return false;
			}
		}
		if (checkEarth) {
			for (int i = 0; i < 4; i++) {
				if (earth[x + 4][y + i]->checkActive() == true) return false;
			}
		}
		break;

	case Actor::left:
		if (x <= 0) return false; // Can't move left, out of bounds
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			for (int i = -3; i <= 3; i++) {
				if ((*it)->getX() == x - 4 && (*it)->getY() == y + i && (*it)->getBlockable() == Actor::walkBlocking) return false;
			}
		}
		if (checkEarth) {
			for (int i = 0; i < 4; i++) {
				if (earth[x - 1][y + i]->checkActive() == true) return false;
			}
		}
		break;

	case Actor::up:
		if (y >= 60) return false; // Can't move up, out of bounds
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			for (int i = -3; i <= 3; i++) {
				if ((*it)->getX() == x + i && (*it)->getY() == y + 4 && (*it)->getBlockable() == Actor::walkBlocking) return false;
			}
		}
		if (checkEarth) {
			for (int i = 0; i < 4; i++) {
				if (earth[x + i][y + 4]->checkActive() == true) return false;
			}
		}
		break;

	case Actor::down:
		if (y <= 0) return false; // Can't move left, out of bounds
		for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
			for (int i = -3; i <= 3; i++) {
				if ((*it)->getX() == x + i && (*it)->getY() == y - 4 && (*it)->getBlockable() == Actor::walkBlocking) return false;
			}
		}
		if (checkEarth) {
			for (int i = 0; i < 4; i++) {
				if (earth[x + i][y - 1]->checkActive() == true) return false;
			}
		}
		break;
	}
	return true;
}

void StudentWorld::useSonar(const int& x, const int& y) {
	for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
		if ((*it)->within((*it)->getX(), (*it)->getY(), x, y, 12))
			(*it)->setActive(true);
	}
}

bool StudentWorld::annoyActors(const int& x, const int& y, const int& annoyAmount, const bool& usingSquirt) {
	bool flag = false;
	if (player->getX() - x <= 3 && x - player->getX() <= 3 
		&& player->getY() - y <= 3 && y - player->getY() <= 3 && !usingSquirt) {
		player->getAnnoyed(annoyAmount);
		flag = true;
	}

	for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
		if ((*it)->getX() - x <= 3 && x - (*it)->getX() <= 3 
			&& (*it)->getY() - y <= 3 && y - (*it)->getY() <= 3 && (*it)->getAnnoyable()) {
			(*it)->getAnnoyed(annoyAmount);
			flag = true;
		}
	}
	return flag;
}

bool StudentWorld::bribeActors(const int& x, const int& y) {
	for (vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
		if ((*it)->getX() - x <= 3 && x - (*it)->getX() <= 3
			&& (*it)->getY() - y <= 3 && y - (*it)->getY() <= 3 && (*it)->getBribeable()) {
			(*it)->getBribed();
			return true; // in the for loop so we only bribe one actor.
		}
	}
	return false;
}

void StudentWorld::annoyPlayer(const int& annoyAmount) {
	player->getAnnoyed(annoyAmount);
}

bool StudentWorld::withinPlayer(const int& x, const int& y, const int& distance) {
	return player->within(x, y, player->getX(), player->getY(), distance);
}

void StudentWorld::decBarrels() {
	numBarrels--;
}

void StudentWorld::incGold() {
	player->setGold(player->getGold() + 1);
}

void StudentWorld::incSonars() {
	player->setSonars(player->getSonars() + 2);
}

void StudentWorld::incSquirts() {
	player->setSquirts(player->getSquirts() + 5);
}

void StudentWorld::decNumProtesters() {
	numProtesters--;
}

void StudentWorld::addProtester() {
	int temp = rand() % 100;
	if (temp <= probOfHardcore) {
		actors.push_back(new HardcoreProtester(this));
	}
	else {
		actors.push_back(new RegularProtester(this));
	}
	numProtesters++;
}

void StudentWorld::addRandomGoodie() {
	int temp = rand() % chanceOfGoodie;
	if (temp == 1) {	// if we selected one number from 1 to chanceOfGoodie,
						// there is a 1 in chanceOfGoodie chance to occur. 
		int i = rand() % 5;
		if (i == 1) {	// if we selected 1 from 0-4, aka 1 in 5 chance, then add sonar
			actors.push_back(new SonarKit(0, 60, this));
		}
		else {
			int x, y;
			while (true) {

				x = rand() % 61; // select (x,y) randomly in field
				y = rand() % 61;
				if (spotHasEarth(x, y))
					continue;
				else
					break;

			}
			actors.push_back(new WaterPool(x, y, this));
		}
	}
}

bool StudentWorld::spotHasEarth(const int& x, const int& y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (earth[x + i][y + j]->checkActive() == true)
				return true;
		}
	}
	return false;
}

bool StudentWorld::isFacingPlayer(const int& x, const int& y, const GraphObject::Direction& d) {
	switch (d) {
	case GraphObject::up:
		if (withinPlayer(x, y, 4)) { //TODO: maybe chance this to check manually rather than withinPlayer?
			if (0 <= player->getY() - y && player->getY() - y <= 4)
				return true;
		}
		break;
	case GraphObject::down:
		if (withinPlayer(x, y, 4)) {
			if (0 <= y - player->getY() && y - player->getY() <= 4)
				return true;
		}
		break;
	case GraphObject::left:
		if (withinPlayer(x, y, 4)) {
			if (0 <= x - player->getX() && x - player->getX() <= 4)
				return true;
		}
		break;
	case GraphObject::right:
		if (withinPlayer(x, y, 4)) {
			if (0 <= player->getX() - x && player->getX() - x <= 4)
				return true;
		}
		break;
	}
	return false;
}

bool StudentWorld::inLineWithPlayer(const int& x, const int& y, GraphObject::Direction& d) {
	if (x == player->getX()) { // same column
		if (player->getY() - y > 0) { // player is higher than protester
			for (int i = 0; i < player->getY() - y; i++) {
				if (!canMove(x, y + i, GraphObject::up, true)) { // check if we can move directly to player
					return false;
				}
			}
			d = GraphObject::up;
		}
		else {
			for (int i = 0; i < y - player->getY(); i++) {
				if (!canMove(x, y - i, GraphObject::down, true)) { 
					return false;
				}
			}
			d = GraphObject::down;
		}
		return true;
	}
	if (y == player->getY()) { // same row
		if (player->getX() - x > 0) { // player is to the right of protester
			for (int i = 0; i < player->getX() - x; i++) {
				if (!canMove(x + i, y, GraphObject::right, true)) {
					return false;
				}
			}
			d = GraphObject::right;
		}
		else {
			for (int i = 0; i < x - player->getX(); i++) {
				if (!canMove(x - i, y, GraphObject::left, true)) {
					return false;
				}
			}
			d = GraphObject::left;
		}
		return true;
	}
	return false;
}

std::stack<GraphObject::Direction> StudentWorld::solveMazeToPlayer(Protester* prot) {
	return prot->solveMaze(player->getX(), player->getY());
}