/*
 * This file provides implementtations of the read only wrappers for GameState,
 * SquareState, and Player.
 */

#include "aiplayer.h"
#include "gamestate.h"

ROPlayer::ROPlayer(pos_t x, pos_t y, const Player &p)
	: xoff(x)
	, yoff(y)
	, player(p)
{
}

plid_t ROPlayer::getId() const
{
	return player.getId();
}

pos_t ROPlayer::getX() const
{
	if (!isVisible())
		return OUT_OF_VIEW;

	return player.getX() - xoff;
}

pos_t ROPlayer::getY() const
{
	if (!isVisible())
		return OUT_OF_VIEW;

	return player.getY() - xoff;
}

bool ROPlayer::isVisible() const
{
	int x = player.getX() - xoff;
	int y = player.getY() - yoff;

	return -16 < x && x < 16 && -16 < y && y < 16;
}

Direction ROPlayer::getDirection() const
{
	if (!isVisible())
		return Direction::NONE;

	return player.getActualDirection();
}

quint8 ROPlayer::getScore() const
{
	return player.getScore();
}

ROSquareState::ROSquareState(pos_t xp, pos_t yp, const SquareState &s, const ROGameState &rs)
	: x(xp)
	, y(yp)
	, state(s)
	, rgs(rs)
{
}

pos_t ROSquareState::getX() const
{
	return x;
}

pos_t ROSquareState::getY() const
{
	return y;
}

bool ROSquareState::hasTrail() const
{
	return state.hasTrail();
}

TrailType ROSquareState::getTrailType() const
{
	return state.getTrailType();
}

plid_t ROSquareState::getTrailPlayerId() const
{
	return state.getTrailPlayerId();
}

ROPlayer *ROSquareState::getTrailPlayer() const
{
	return rgs.lookupPlayer(getTrailPlayerId());
}

bool ROSquareState::isOccupied() const
{
	return state.isOccupied();
}

plid_t ROSquareState::getOccupyingPlayerId() const
{
	return state.getOccupyingPlayerId();
}

ROPlayer *ROSquareState::getOccupyingPlayer() const
{
	return rgs.lookupPlayer(getOccupyingPlayerId());
}

bool ROSquareState::isOwned() const
{
	return state.isOwned();
}

plid_t ROSquareState::getOwningPlayerId() const
{
	return state.getOwningPlayerId();
}

ROPlayer *ROSquareState::getOwningPlayer() const
{
	return rgs.lookupPlayer(getOwningPlayerId());
}

ROGameState::ROGameState(pos_t x, pos_t y, GameState &s)
	: xoff(x)
	, yoff(y)
	, gs(s)
{
	for (auto iter = gs.players.begin(); iter != gs.players.end(); iter++)
		if (iter.value())
			pls.insert(iter.key(), new ROPlayer(xoff, yoff, *iter.value()));
}

ROGameState::~ROGameState()
{
	foreach (ROPlayer *rpl, pls)
		if (rpl)
			delete rpl;
}

tick_t ROGameState::getTick() const
{
	return gs.getTick();
}

ROSquareState ROGameState::getState(pos_t x, pos_t y) const
{
	if (x < -15 || x > 15 || y < -15 || y > 15)
		return ROSquareState(x, y, gs.getState(-2, -2), *this);

	return ROSquareState(x, y, gs.getState(x + xoff, y + yoff), *this);
}

ROPlayer *ROGameState::lookupPlayer(plid_t pid) const
{
	if (pls.contains(pid))
		return pls.value(pid);
	else
		return NULL;
}
