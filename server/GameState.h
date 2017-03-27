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

#include <vector>

#include "Player.h"
#include "types.h"

const plid_t NULL_ID = 0;
const plid_t UNOCCUPIED = 0;
const plid_t OUT_OF_BOUNDS = 255;

class GameState;

enum TrailType 
{
	NOTRAIL = 0,
	EASTTOWEST = 1,
	NORTHTOSOUTH = 2,
	NORTHTOEAST = 3,
	NORTHTOWEST = 4,
	SOUTHTOEAST = 5,
	SOUTHTOWEST = 6,
};

class SquareState 
{
friend class GameState;
public:
	pos_t getX();
	pos_t getY();

	TrailType getTrailType();
	void setTrailType(TrailType trail);

	bool hasTrail();
	Player *getTrailPlayer();
	void setTrailPlayer(Player *player);
	void setTrailPlayerId(plid_t id);
	plid_t getTrailPlayerId();

    // We define "occupying" to mean having a trail
	bool isOccupied();
	Player *getOccupyingPlayer();
	void setOccupyingPlayer(Player *player);
	plid_t getOccupyingPlayerId();
	void setOccupyingPlayerId(plid_t player);

    // We define "owning" to mean being in your body
	bool isOwned();
	Player *getOwningPlayer();
	void setOwningPlayer(Player *player);
	plid_t getOwningPlayerId();
	void setOwningPlayerId(plid_t player);

	// "flooded" flag is for the filling algorithms
	void markAsFlooded();
	bool isFlooded();

private:
	state_t *state;
	bool *changed;
	bool flooded;

	SquareState();
	SquareState(state_t *state, bool *changed);
};

class GameState 
{
public:
	pos_t getWidth();
	pos_t getHeight();

	SquareState getState(pos_t x, pos_t y);

	Player *lookupPlayer(plid_t id);

	const std::vector<Player> *getPlayers();

private:
	pos_t width;
	pos_t height;

	state_t **board;

	GameState();
	GameState(pos_t width, pos_t height);
};

#endif // !GAMESTATE_H
