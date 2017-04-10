/*
 * Implements SquareState---basically, some user friendly wrappers around
 * bitwise calls.
 */

#include "gamestate.h"

SquareState::SquareState(const GameState &gst, pos_t px, pos_t py, state_t &st, state_t &df, quint8 &fl)
	: gs(gst)
	, x(px)
	, y(py)
	, state(st)
	, diff(df)
	, flags(fl)
{
}

pos_t SquareState::getX() const
{
	return x;
}

pos_t SquareState::getY() const
{
	return y;
}

bool SquareState::hasTrail() const
{
	return getTrailType() != TrailType::NOTRAIL;
}

TrailType SquareState::getTrailType() const
{
	return TrailType(state & 0x07);
}

void SquareState::setTrailType(TrailType t)
{
	if (getTrailPlayerId() == OUT_OF_BOUNDS)
		return;

	state_t change = (state & 0x07) ^ t;
	state ^= change;
	diff ^= change;
}

plid_t SquareState::getTrailPlayerId() const
{
	return static_cast<plid_t>((state & 0xFF00) >> 8);
}

void SquareState::setTrailPlayerId(plid_t id)
{
	if (id == OUT_OF_BOUNDS || getTrailPlayerId() == OUT_OF_BOUNDS)
		return;

	state_t change = (state & 0xFF00) ^ (static_cast<state_t>(id) << 8);
	state ^= change;
	diff ^= change;
}

Player *SquareState::getTrailPlayer() const
{
	return gs.lookupPlayer(getTrailPlayerId());
}

void SquareState::setTrailPlayer(Player *player)
{
	if (player)
		setTrailPlayerId(player->getId());
	else
		setTrailPlayerId(UNOCCUPIED);
}

bool SquareState::isOccupied() const
{
	plid_t id = getOccupyingPlayerId();
	return id != UNOCCUPIED;
}

plid_t SquareState::getOccupyingPlayerId() const
{
	return static_cast<plid_t>((state & 0xFF0000) >> 16);
}

void SquareState::setOccupyingPlayerId(plid_t id)
{
	if (id == OUT_OF_BOUNDS || getOccupyingPlayerId() == OUT_OF_BOUNDS)
		return;

	state_t change = (state & 0xFF0000) ^ (static_cast<state_t>(id) << 16);
	state ^= change;
	diff ^= change;
}

Player *SquareState::getOccupyingPlayer() const
{
	return gs.lookupPlayer(getOccupyingPlayerId());
}

void SquareState::setOccupyingPlayer(Player *player)
{
	if (player)
		setOccupyingPlayerId(player->getId());
	else
		setOccupyingPlayerId(UNOCCUPIED);
}

bool SquareState::isOwned() const
{
	plid_t id = getOwningPlayerId();
	return id != UNOCCUPIED;
}

plid_t SquareState::getOwningPlayerId() const
{
	return static_cast<plid_t>((state & 0xFF000000) >> 24);
}

void SquareState::setOwningPlayerId(plid_t id)
{
	if (id == OUT_OF_BOUNDS || getOwningPlayerId() == OUT_OF_BOUNDS)
		return;

	state_t change = (state & 0xFF000000) ^ (static_cast<state_t>(id) << 24);
	state ^= change;
	diff ^= change;
}

Player *SquareState::getOwningPlayer() const
{
	return gs.lookupPlayer(getOwningPlayerId());
}

void SquareState::setOwningPlayer(Player *player)
{
	if (player)
		setOwningPlayerId(player->getId());
	else
		setOwningPlayerId(UNOCCUPIED);
}

bool SquareState::isOutOfBounds() const
{
	plid_t id = getOwningPlayerId();
	return id == OUT_OF_BOUNDS;
}

bool SquareState::isFlooded() const
{
	return flags & 0x01;
}

void SquareState::markAsFlooded()
{
	flags |= 0x01;
}

void SquareState::markAsUnflooded()
{
	flags &= 0xFE;
}

bool SquareState::hasBeenChecked() const
{
	return flags & 0x02;
}

void SquareState::markAsChecked()
{
	flags |= 0x02;
}

void SquareState::markAsUnchecked()
{
	flags &= 0xFC;
}

Direction SquareState::getDirection() const
{
	return Direction((state & 0x38) >> 3);
}

void SquareState::setDirection(Direction d)
{
	state_t change = (state & 0x38) ^ (static_cast<state_t>(d) << 3);
	state ^= change;
	diff ^= change;
}
