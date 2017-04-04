/*
 * Implements ClientSquareState---basically, some user friendly wrappers around
 * bitwise calls.
 */

#include "clientgamestate.h"

ClientSquareState::ClientSquareState(const ClientGameState &gst, pos_t px, pos_t py, const state_t &st)
	: gs(gst)
	, x(px)
	, y(py)
	, state(st)
{
}

pos_t ClientSquareState::getX() const
{
	return x;
}

pos_t ClientSquareState::getY() const
{
	return y;
}

bool ClientSquareState::hasTrail() const
{
	return getTrailType() != TrailType::NOTRAIL;
}

TrailType ClientSquareState::getTrailType() const
{
	return TrailType(state & 0x07);
}

plid_t ClientSquareState::getTrailPlayerId() const
{
	return static_cast<plid_t>((state & 0xFF00) >> 8);
}

ClientPlayer *ClientSquareState::getTrailPlayer() const
{
	return gs.lookupPlayer(getTrailPlayerId());
}

bool ClientSquareState::isOccupied() const
{
	plid_t id = getOccupyingPlayerId();
	return id != UNOCCUPIED;
}

plid_t ClientSquareState::getOccupyingPlayerId() const
{
	return static_cast<plid_t>((state & 0xFF0000) >> 16);
}

ClientPlayer *ClientSquareState::getOccupyingPlayer() const
{
	return gs.lookupPlayer(getOccupyingPlayerId());
}

bool ClientSquareState::isOwned() const
{
	plid_t id = getOwningPlayerId();
	return id != UNOCCUPIED;
}

plid_t ClientSquareState::getOwningPlayerId() const
{
	return static_cast<plid_t>((state & 0xFF000000) >> 24);
}

ClientPlayer *ClientSquareState::getOwningPlayer() const
{
	return gs.lookupPlayer(getOwningPlayerId());
}

Direction ClientSquareState::getDirection() const
{
	return Direction((state & 0x38) >> 3);
}
