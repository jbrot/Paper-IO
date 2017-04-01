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

#include <QtCore>
#include <vector>

#include "Player.h"
#include "types.h"

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
	pos_t getX() const;
	pos_t getY() const;

	TrailType getTrailType() const;
	void setTrailType(TrailType trail);

	bool hasTrail() const;
	plid_t getTrailPlayerId() const;
	void setTrailPlayerId(plid_t id);
	Player *getTrailPlayer() const;
	void setTrailPlayer(Player *player);

    // A square is occupied if the specified player's current position
    // is this square. 
	bool isOccupied() const;
	plid_t getOccupyingPlayerId() const;
	void setOccupyingPlayerId(plid_t player);
	Player *getOccupyingPlayer() const;
	void setOccupyingPlayer(Player *player);

    // We define "owning" to mean being in your body
	bool isOwned() const;
	plid_t getOwningPlayerId() const;
	void setOwningPlayerId(plid_t player);
	Player *getOwningPlayer() const;
	void setOwningPlayer(Player *player);

	// "flooded" flag is for the filling algorithms
	bool isFlooded() const;
	void markAsFlooded();
	void markAsUnflooded();

	// Marking squares as checked for flood algorithm
	bool hasBeenChecked() const;
	void markAsChecked();
	void markAsUnchecked();

private:
	GameState &gs;
	pos_t x;
	pos_t y;
	state_t &state;
	state_t &diff;
	quint8 &flags;

	SquareState(GameState &gs, pos_t x, pos_t y, state_t &state, state_t &diff, quint8 &flags);
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
