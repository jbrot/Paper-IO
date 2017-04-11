/*
 * The various classes provided here represent the client's view of the
 * game state. Their values are set in iohandler and processed by the renderer.
 */
#ifndef CLIENTGAMESTATE_H
#define CLIENTGAMESTATE_H

#include <QMutex>

#include "protocol.h"

class Client;
class ClientGameState;
class GameWidget;
class IOHandler;

class ClientPlayer
{
friend class ClientGameState;
friend class IOHandler;
public:
	plid_t getId() const;
	QString getName() const;

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

	/*
	 * A player's score is stored as an unsigned 16 bit integer ranging from
	 * 0 to whatever, where the player's score in percentage of the board controlled
	 * can be found by dividing this number by how many valid squares there are.
	 */
	score_t getScore() const;

private:
	const ClientGameState &gs;
	const plid_t id;
	const QString name;

	pos_t x;
	pos_t y;
	score_t score;

	void setX(pos_t x);
	void setY(pos_t y);

	void setScore(score_t score);

	ClientPlayer(const ClientGameState &gs, const plid_t id, const QString &name, pos_t x, pos_t y);
};

class ClientSquareState
{
friend class ClientPlayer;
friend class ClientGameState;
public:
	pos_t getX() const;
	pos_t getY() const;

	/*
	 * A square has a trail if a player has gone through it after
	 * leaving their territory but has not yet returned to their territory/
	 *
	 * WARNING: If a square is outside of the game board, it will report
	 * false for hasTrail(), NOTRAIL for getTrailType(), OUT_OF_BOUNDS for
	 * getTrailPlayerId(), and NULL for getTrailPlayer().
	 */
	bool hasTrail() const;
	TrailType getTrailType() const;
	plid_t getTrailPlayerId() const;
	ClientPlayer *getTrailPlayer() const;

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
	ClientPlayer *getOccupyingPlayer() const;

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
	ClientPlayer *getOwningPlayer() const;
private:
	const ClientGameState &gs;
	const pos_t x;
	const pos_t y;
	const state_t &state;

	Direction getDirection() const;

	ClientSquareState(const ClientGameState &gs, pos_t x, pos_t y, const state_t &state);
};

class ClientGameState
{
friend class Client;
friend class GameWidget;
friend class IOHandler;
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
	ClientSquareState getState(pos_t x, pos_t y) const;

	ClientPlayer *lookupPlayer(plid_t id) const;

	std::vector<ClientPlayer> getPlayers() const;

	plid_t getClientId() const;
	ClientPlayer *getClient() const;

	quint16 getTotalSquares() const;

private:
	QMutex lock;

	QHash<plid_t, ClientPlayer *> players;
	tick_t tick;

	std::pair<plid_t, score_t> leaderboard[10];

	state_t board[CLIENT_FRAME][CLIENT_FRAME];
	quint16 totalSquares;

	plid_t client;

	void lockState();
	void unlock();

	ClientGameState();
	~ClientGameState();
};

#endif // !CLIENTGAMESTATE_H
