/*
 * Implements ClientGameState
 */

#include "clientgamestate.h"
#include "protocol.h"

ClientGameState::ClientGameState()
	: lock()
	, players()
	, tick(0)
	, client(NULL_ID)
{
	std::fill(leaderboard, leaderboard + 10, 0);
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

std::vector<ClientPlayer> ClientGameState::getPlayers() const
{
	std::vector<ClientPlayer> pls;
	foreach (ClientPlayer *pl, players)
	{
		if (pl)
			pls.push_back(*pl);
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

void ClientGameState::lockState()
{
	lock.lock();
}

void ClientGameState::unlock()
{
	lock.unlock();
}