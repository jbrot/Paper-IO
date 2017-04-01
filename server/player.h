/**
 * Player.h
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Represents a player
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "types.h"

class Player
{
public:
	plid_t getId();
	std::string getName();

	void setX(pos_t x);
	pos_t getX();

	void setY(pos_t y);
	pos_t getY();

	void setLocation(pos_t x, pos_t y);

    Direction getNewDirection();
    void setNewDirection(Direction newD);

    Direction getOldDirection();
    void setOldDirection(Direction old);

	bool isDead();
	void setDead(bool dead);

	bool setWinner();

private:
	std::string name;
	plid_t id;
	pos_t x;
	pos_t y;
    Direction oldDir;
    Direction newDir;
	bool dead;
	bool winner;

	Player();
};
#endif // !PLAYER_H
