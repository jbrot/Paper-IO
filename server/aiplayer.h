/*
 * This class represents an AI Player in the game.
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <QHash>
#include <vector>

#include "types.h"

class GameHandler;

class GameState;
class SquareState;
class Player;

class ROGameState;
class ROSquareState;

/*
 * This positiion in either x or y means the object
 * is out of view.
 */
pos_t OUT_OF_VIEW = 32767;

class ROPlayer
{
friend class ROGameState;
public:
	plid_t getId() const;

	/*
	 * These coordinates are relative to the player. If they are not
	 * in range, they will both report OUT_OF_VIEW.
	 */
	pos_t getX() const;
	pos_t getY() const;
	bool isVisible() const;

	/*
	 * If the player is out of view this returns Direction::NONE.
	 */
	Direction getDirection() const;

	bool isDead() const;

	/*
	 * A player's score is stored as an unsigned 8 bit integer ranging from 
	 * 0 to 200, where the player's score in percentage of the board controlled
	 * can be found by dividing this number in two.
	 */
	quint8 getScore() const;

private:
	const pos_t xoff;
	const pos_t yoff;
	const Player &player;

	ROPlayer(pos_t xoff, pos_t yoff, const Player &pl);
};

class ROSquareState
{
friend class ROGameState;
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
	 */
	bool hasTrail() const;
	TrailType getTrailType() const;
	plid_t getTrailPlayerId() const;
	ROPlayer *getTrailPlayer() const;

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
	ROPlayer *getOccupyingPlayer() const;

	/*
	 * A square is owned if it is in a player's "body"---that is, the square is
	 * either one of the initial nine squares granted to a player or part of
	 * additional territory that the player has fully surrounded.
	 *
	 * WARNING: If a square is outside of the game board, it will report true
	 * for isOwned(), OUT_OF_BOUNDS for getOwningPlayerId(), and NULL for
	 * getOwningPlayer(). Furthermore, setOwningPlayerId() and setOwningPlayer()
	 * will have no effect.
	 */
	bool isOwned() const;
	plid_t getOwningPlayerId() const;
	ROPlayer *getOwningPlayer() const;
private:
	const pos_t x;
	const pos_t y;
	const SquareState &state;
	const ROGameState &rgs;

	ROSquareState(pos_t x, pos_t y, const SquareState &state, const ROGameState &rgs);
};

class ROGameState
{
friend class GameHandler;
public:
	tick_t getTick() const;

	/*
	 * WARNING: If the coordinates passed to getState() are out of bounds, getState()
	 * will return an out of bounds SquareState which means the values it will report
	 * might not be what are expected (see SquareState for details). An out of bounds
	 * SquareState cannot have any of its state properties changed (i.e., trail,
	 * owner, occupier). However, if the SquareState is adjacent (including diagonally
	 * adjacent) to an in bounds SquareState, its flags can still be used as normal.
	 * If an out of bounds SquareState is not adjacent to an in bounds SquareState,
	 * then the behavior of its flags is undefined.
	 */
	ROSquareState getState(pos_t x, pos_t y) const;

	ROPlayer *lookupPlayer(plid_t id) const;

	/*
	 * WARNING: This function is relatively inneficient and should
	 * be used sparingly.
	 */
	std::vector<ROPlayer> getPlayers() const;
private:
	const pos_t xoff;
	const pos_t yoff;
	GameState &gs;
	QHash<plid_t, ROPlayer *> pls;

	ROGameState(pos_t xoff, pos_t yoff, GameState &gs);
	~ROGameState();
};

class AIPlayer
{
public:
	AIPlayer();

	Direction tick(const ROGameState &gs);
};

#endif // !AIPLAYER_H
