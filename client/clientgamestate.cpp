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
	, kiosk(0)
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

const ClientPlayer *ClientGameState::lookupPlayer(plid_t id) const
{
	return const_cast<ClientGameState *>(this)->lookupPlayer(id);
}

ClientPlayer *ClientGameState::lookupPlayer(plid_t id)
{
	if (!players.contains(id))
		return NULL;
	return players.value(id);
}

QList<const ClientPlayer *> ClientGameState::getPlayers() const
{
    QList<const ClientPlayer *> pls;
	foreach (ClientPlayer *pl, players)
		if (pl)
			pls.push_back(pl);

	return pls;
}

plid_t ClientGameState::getClientId() const
{
	return client;
}

const ClientPlayer *ClientGameState::getClient() const
{
	return lookupPlayer(client);
}

ClientPlayer *ClientGameState::getClient()
{
	return lookupPlayer(client);
}

quint16 ClientGameState::getTotalSquares() const
{
	return totalSquares;
}

const std::pair<plid_t, score_t> *ClientGameState::getLeaderboard() const
{
	return leaderboard;
}

int ClientGameState::kioskMode() const
{
	return kiosk;
}

void ClientGameState::lockState()
{
	lock.lock();
}

void ClientGameState::unlock()
{
	lock.unlock();
}
