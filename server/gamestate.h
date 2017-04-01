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
#include <QString>
#include <vector>

#include "types.h"

class GameHandler;
class GameState;
class SquareState;

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

class Player
{
friend class GameState;
public:
	plid_t getId() const;

	/*
	 * setX(), setY(), and setLocation() return true if the location
	 * change was successful and false otherwise. Note that a location
	 * change will only fail if the new location reports it is occupied.
	 * This will happen if either there is another player in this location
	 * or if it is out of bounds.
	 */
	pos_t getX() const;
	bool setX(pos_t x);

	pos_t getY() const;
	bool setY(pos_t y);

	bool setLocation(pos_t x, pos_t y);

	/*
	 * The new direction is the direction the client has
	 * requested to move in this tick.
	 *
	 * WARNING: There is no guarantee that the new direction is valid.
	 * This must be checked before changing actual direction.
	 */
	Direction getNewDirection() const;

	/*
	 * The actual direction is the direction the player
	 * is moving in. This is the state that is sent to
	 * clients.
	 */
	Direction getActualDirection() const;
	void setActualDirection(Direction old);

	bool isDead() const;
	void setDead(bool dead);

	bool isWinner() const;
	void setWinner();

private:
	const GameState &gs;
	const plid_t id;

	pos_t x;
	pos_t y;
    Direction newDir;
    Direction dir;
	bool dead;
	bool winner;

	Player(const GameState &gs, const plid_t id, pos_t x, pos_t y);
};

class SquareState 
{
friend class GameState;
friend class Player;
public:
	pos_t getX() const;
	pos_t getY() const;


	/*
	 * A square has a trail if a player has gone through it after
	 * leaving their territory but has not yet returned to their territory/
	 *
	 * WARNING: If a square is outsife of the game board, it will report
	 * false for hasTrail(), NOTRAIL for getTrailType(), OUT_OF_BOUNDS for
	 * getTrailPLayerId(), and NULL for getTrailPlayer(). Furthermore,
	 * setTrailType(), setTrailPlayerId(), and setTrailPlayer() will have
	 * no effect.
	 *
	 * WARNING: setTraiilPlayerId(OUT_OF_BOUNDS) will have no effect as this
	 * is an invalid call.
	 */
	bool hasTrail() const;
	TrailType getTrailType() const;
	void setTrailType(TrailType trail);
	plid_t getTrailPlayerId() const;
	void setTrailPlayerId(plid_t id);
	Player *getTrailPlayer() const;
	void setTrailPlayer(Player *player);

    /*
     * A square is occupied if the specified player's current position
     * is this square. To make a square unoccupied, use the setLocation
     * method of the occupying player.
     *
     * WARNING: If a square is outside of the game board, it will report true
     * for isOccupied(), OUT_OF_BOUNDS for getOccupyingPlayerId(), and NULL
     * for getOccupyingPlayer().
     */
	bool isOccupied() const;
	plid_t getOccupyingPlayerId() const;
	Player *getOccupyingPlayer() const;

	/*
	 * A square is owned if it is in a player's "body"---that is, the square is
	 * either one of the initial nine squares granted to a player or part of
	 * additional territory that the player has fully surrounded.
	 *
	 * WARNING: If a square is outside of the game board, it will report true
	 * for isOwned(), OUT_OF_BOUNDS for getOwningPlayerId(), and NULL for
	 * getOwningPlayer(). Furthermore, setOwningPlayerId() and setOwningPlayer()
	 * will have no effect.
	 *
	 * WARNING: setOwningPlayerId(OUT_OF_BOUNDS) will have no effect as this is an
	 * invalid call.
	 */
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
	const GameState &gs;
	const pos_t x;
	const pos_t y;
	state_t &state;
	state_t &diff;
	quint8 &flags;

	Direction getDirection() const;
	void setDirection(Direction d);

	void setOccupyingPlayerId(plid_t player);
	void setOccupyingPlayer(Player *player);

	SquareState(const GameState &gs, pos_t x, pos_t y, state_t &state, state_t &diff, quint8 &flags);
};

class GameState 
{
friend class GameHandler;
public:
	pos_t getWidth() const;
	pos_t getHeight() const;

	/*
	 * Note: These two functions are marked const because they do not affect the 
	 * GameState directly. However, any changes made in the objects returned
	 * WILL affect the GameState.
	 *
	 * WARNING: If the coordinates passed to getState() are out of bounds, getState()
	 * will return an out of bounds SquareState which means the values it will report
	 * might not be what are expected (see SquareState for details). An out of bounds
	 * SquareState cannot have any of its state properties changed (i.e., trail,
	 * owner, occupier). However, if the SquareState is adjacent (including diagonally
	 * adjacent) to an in bounds SquareState, its flags can still be used as normal.
	 * If an out of bounds SquareState is not adjacent to an in bounds SquareState,
	 * then the behavior of its flags is undefined.
	 */
	SquareState getState(pos_t x, pos_t y) const;
	Player *lookupPlayer(plid_t id) const;

	const std::vector<Player> &getPlayers() const;

private:
	const pos_t width;
	const pos_t height;

	state_t **board;
	state_t **diff;
	quint8 **flags;

	GameState();
	GameState(pos_t width, pos_t height);
	~GameState();
};

#endif // !GAMESTATE_H
