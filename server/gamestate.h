/**
 * gamestate.h
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

#include <QHash>
#include <QReadWriteLock>
#include <QtCore>
#include <vector>

#include "types.h"

class ClientHandler;
class GameHandler;
class ROGameState;

class GameState;
class SquareState;

class Player
{
friend class GameState;
friend class GameHandler;
public:
	plid_t getId() const;
	QString getName() const;

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
	void setActualDirection(Direction dir);

	bool isDead() const;

	/*
	 * Marks the player as dead and removes the player object from the board.
	 * This frees the player's current location so another player can occupy it.
	 *
	 * WARNING: This does not remove the player's trail or the player's territory.
	 * They must be removed separately in the same tick as this function is called
	 * or undefined behavior may occur.
	 */
	void kill();

	/*
	 * A player's score is stored as an unsigned 16 bit integer ranging from
	 * 0 to whatever, where the player's score in percentage of the board controlled
	 * can be found by dividing this number by how many valid squares there are.
	 */
	quint16 getScore() const;
	void setScore(quint16 score);

private:
	GameState &gs;
	const plid_t id;
	const QString name;

	pos_t x;
	pos_t y;
	Direction newDir;
	quint16 score;
	bool dead;

	Player(GameState &gs, const plid_t id, const QString &name, pos_t x, pos_t y);
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
	 * WARNING: If a square is outside of the game board, it will report
	 * false for hasTrail(), NOTRAIL for getTrailType(), OUT_OF_BOUNDS for
	 * getTrailPLayerId(), and NULL for getTrailPlayer(). Furthermore,
	 * setTrailType(), setTrailPlayerId(), and setTrailPlayer() will have
	 * no effect.
	 *
	 * WARNING: setTrailPlayerId(OUT_OF_BOUNDS) will have no effect as this
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

	bool isOutOfBounds() const;

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
friend class ClientHandler;
friend class GameHandler;
friend class Player;
friend class ROGameState;
public:
	pos_t getWidth() const;
	pos_t getHeight() const;

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
	SquareState getState(pos_t x, pos_t y) const;

	Player *lookupPlayer(plid_t id) const;

	/*
	 * WARNING: This function is relatively inneficient and should
	 * be used sparingly.
	 */
	std::vector<Player> getPlayers() const;


private:
	const pos_t width;
	const pos_t height;

	QReadWriteLock lock;

	QHash<plid_t, Player *> players;
	bool playersChanged;
	tick_t tick;

	// Since plid_t = quint8 = "score"
	quint8 leaderboard[10];
	bool scoresChanged;
	bool leaderboardChanged;

	state_t *boardStart;
	state_t **board;
	state_t *diffStart;
	state_t **diff;
	quint8 **flags;

	/*
	 * If width and height are less than one, bad things will happen.
	 * In general they should both be at least 15. If they are too
	 * close to the upper bound of pos_t, bad things will also happen.
	 * However, the board should never be anywhere close to that large
	 * for memory reasons.
	 */
	GameState(pos_t width, pos_t height);
	~GameState();

	void nextTick();

	/*
	 * Adds the player at the specified location. Note this only adds the
	 * player, it does not provide the starting territory. If the specified
	 * location already contains a player object or is out of bounds, this
	 * function returns false. If a player with the given id already exists,
	 * this function returns false. If the player is successfully added,
	 * this function returns true.
	 */
	bool addPlayer(plid_t id, const QString &name, pos_t x, pos_t y);
	/*
	 * Immediately removes the player from the game.
	 *
	 * WARNING: This function does not remove the player's territory or trail.
	 * They MUST be removed separately within the tick, or behavior is undefined.
	 */
	QHash<plid_t, Player *>::iterator removePlayer(QHash<plid_t, Player *>::iterator i);

	bool havePlayersChanged() const;

	void markScoresChanged();
	bool haveScoresChanged() const;

	void recomputeLeaderboard();
	bool hasLeaderboardChanged() const;

	/*
	 * Interface with the internal lock.
	 */
	void lockForRead();
	void lockForWrite();
	void unlock();
};

Q_DECLARE_METATYPE(GameState *)

#endif // !GAMESTATE_H
