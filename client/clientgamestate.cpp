/*
 * Implements ClientGameState
 */

#include "clientgamestate.h"
#include "protocol.h"

ClientGameState::ClientGameState()
	: lock()
	, players()
	, tick(0)
	, tickRate(0)
	, lastTick()
	, client(NULL_ID)
{
	std::fill(leaderboard, leaderboard + 5, std::make_pair(NULL_ID, 0));
	std::fill(board[0], board[0] + CLIENT_FRAME * CLIENT_FRAME, 0);
}

ClientGameState::~ClientGameState()
{
	foreach(ClientPlayer *pl, players)
		delete pl;
}

tick_t ClientGameState::getTick() const
{
	return tick;
}

quint16 ClientGameState::getTickRate() const
{
	return tickRate;
}

QDateTime ClientGameState::getLastTick() const
{
	return lastTick;
}

ClientSquareState ClientGameState::getState(pos_t x, pos_t y) const
{
	pos_t rx = x + (CLIENT_FRAME / 2);
	pos_t ry = y + (CLIENT_FRAME / 2);
	if (0 <= rx && rx < CLIENT_FRAME && 0 <= ry && ry < CLIENT_FRAME)
		return ClientSquareState(*this, rx, ry, board[ry][rx]);

	return ClientSquareState(*this, OUT_OF_BOUNDS, OUT_OF_BOUNDS, OUT_OF_BOUNDS_STATE);
}

ClientPlayer *ClientGameState::lookupPlayer(plid_t id) const
{
	if (!players.contains(id))
		return NULL;
	return players.value(id);
}

QList<ClientPlayer *> ClientGameState::getPlayers() const
{
    QList<ClientPlayer *> pls;
	foreach (ClientPlayer *pl, players)
	{
		if (pl)
			pls.push_back(pl);
	}

	return pls;
}

plid_t ClientGameState::getClientId() const
{
	return client;
}

ClientPlayer *ClientGameState::getClient() const
{
	return lookupPlayer(client);
}

quint16 ClientGameState::getTotalSquares() const
{
	return totalSquares;
}

void ClientGameState::lockState()
{
	lock.lock();
}

void ClientGameState::unlock()
{
	lock.unlock();
}
