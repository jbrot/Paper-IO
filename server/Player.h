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

typedef uint16_t id_t;
typedef uint16_t pos_t;

/*
 * The possible directions the player can take on.
 * Origin is defined to be the top left of the screen
 * ie, positive x is right, positive y is down.
 */
enum Direction 
{
	NONE	 = 0,
	UP  	 = 1,
	DOWN	 = 2,
	LEFT	 = 3,
	RIGHT	 = 4,
};

class Player
{
public:
	id_t getId();
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

private:
	std::string name;
	id_t id;
	pos_t x;
	pos_t y;
    Direction oldDir;
    Direction newDir;
	bool alive;

	Player();
};
#endif // !PLAYER_H
