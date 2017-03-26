/**
 * GameState.h
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Includes two intertwined classes: GameState and SquareState. GameState
 * represents the entire board while SquareState represents a single
 * square. Internally, squares are stored as an array of state_t, which
 * means the information must be accessed via bitwise operations. SquareState
 * provides a more userfriendly way to interface with this.
 */

#ifndef GAMESTATE_H 
#define GAMESTATE_H

#include "Player.h"

typedef uint8_t id_t;
typedef uint16_t pos_t;
typedef uint32_t state_t;

const id_t NULL_ID = 0;
const id_t UNOCCUPIED = 0;
const id_t OUT_OF_BOUNDS = 255;

class GameState;

enum TrailType {
	NONE = 0,
	EASTWEST = 1,
	NORTHSOUTH = 2,
	NORTHEAST = 3,
	NORTHWEST = 4,
	SOUTHEAST = 5,
	SOUTHWEST = 6,
};

class SquareState {
friend class GameState;
public:
	pos_t getX();
	pos_t getY();

	TrailType getTrailType();
	setTrailType(TrailType trail);

	bool hasTrail();
	id_t getTrailPlayer();
	setTrailPlayer(id_t id);

	bool isOccupied();
	id_t getOccupyingPlayer();
	void setOccupyingPlayer(id_t player);

	bool isOwned();
	id_t getOwningPlayer();
	void setOwningPlayer(id_t player);

private:
	state_t *state;
	bool *changed;

	SquareState();
	SquareState(state_t *state, bool *changed);
};

class GameState {
public:
	pos_t getWidth();
	pos_t getHeight();

	SquareState getState(pos_t x, pos_t y);

	Player *lookupPlayer(id_t id);

private:
	pos_t width;
	pos_t height;

	state_t **board;

	GameState();
	GameState(pos_t width, pos_t height);
};

#endif // !GAMESTATE_H
