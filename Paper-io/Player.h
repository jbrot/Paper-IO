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
 */
enum Direction {
	NONE	 = 0,
	UP  	 = 1,
	DOWN	 = 2,
	LEFT	 = 3,
	RIGHT	 = 4,
};

class Player
{
public:
	Player();

	Player(id_t id, std::string name, pos_t x, pos_t y);

	id_t getId();

	std::string getName();

	void setX(pos_t x);
	pos_t getX();

	void setY(pos_t y);
	pos_t getY();

	void setLocation(pos_t x, pos_t y);

private:
	std::string name;
	id_t id;
	pos_t x;
	pos_t y;
	Direction dir;
};
#endif // !PLAYER_H
