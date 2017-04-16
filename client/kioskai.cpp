/*
 * Implementation of the Kiosk AI. Lifted from server/aiplayer.cpp
 */

#include <QtCore>

#include "kioskai.h"

// TODO Perhaps these functions should be put in types.h
// instead of repeated in a few different places.
static int getXOff(Direction d)
{
	switch (d)
	{
	case LEFT:
		return -1;
	case RIGHT:
		return 1;
	default:
		return 0;
	}
}

static int getYOff(Direction d)
{
	switch (d)
	{
	case UP:
		return -1;
	case DOWN:
		return 1;
	default:
		return 0;
	}
}

KioskAI::KioskAI()
	: traillen(0)
{
}

Direction KioskAI::tick(const ClientGameState &cgs)
{
	const ClientPlayer *pl = cgs.getClient();
	if (!pl)
	{
		qCritical() << "Client object is NULL!";
		return Direction::NONE;
	}

	Direction d = pl->getDirection();
	if (d == Direction::NONE)
		return Direction::NONE;

	if (pl->getState().getOwningPlayerId() == pl->getId())
		traillen = 0;
	else
		++traillen;

	// For performance reasons, we don't actually try
	// to predict anything. However, it's too much of a pain
	// to refactor back to using the game state directly.
	double straight = assessDirection(cgs, d, pl->getX() + getXOff(d), pl->getY() + getYOff(d), traillen, 6);

	Direction ld = Direction((d % 4) + 1);
	double left = assessDirection(cgs, ld, pl->getX() + getXOff(ld), pl->getY() + getYOff(ld), traillen, 6);

	Direction rd = Direction(((d + 3) % 4) + 1);
	double right = assessDirection(cgs, rd, pl->getX() + getXOff(rd), pl->getY() + getYOff(rd), traillen, 6);

	if (left >= right && left >= straight)
		return ld;
	else if (right >= left && right >= straight)
		return rd;
	else
		return d;
}

double KioskAI::assessDirection(const ClientGameState &cgs, Direction d, pos_t x, pos_t y, int tl, int recurse)
{
	ClientSquareState state = cgs.getState(x, y);
	// Return 0 if going here would kill us.
	if (state.isOutOfBounds() || state.getTrailPlayerId() == cgs.getClientId()) 
		return 0;

	double ret = 0; 

	// Assess the trail probability. This is either a good or bad
	// thing depending on how aggressibe we are.
	if (state.hasTrail() && state.getTrailPlayerId() != cgs.getClientId())
		ret += 10000;

	// We like to complete trails of length 10, so return a value
	// which gets large quickly around 10 and then tapers off.
	if (state.getOwningPlayerId() == cgs.getClientId()) 
	{
		// Double our aggressiveness twoards players in our territory.
		ret *= 2;

		if (tl > 0)
		{
			const int FALLOFF = 15;
			double coef = tl > FALLOFF ? tl - FALLOFF : 4 * (tl - FALLOFF);
			coef = exp(-(coef * coef));

			return ret + coef * 1000;
		}
	} else {
		++tl;

		// Incentive to leave our territory.
		ret += exp(-(tl * tl)) * 250;
	}

	// At this point, we recurse to assess the surrounding state.
	// N.B. We only predict one level forward because prediction gets really
	// expensive. However, this recursion is cheap so we look 5 moves.
	if (recurse > 0)
	{
		// Check ahead.
		ret += 0.2 * assessDirection(cgs, d, x + getXOff(d), y + getYOff(d), tl, recurse - 1);
		// Check left.
		Direction nd = Direction ((d % 4) + 1);
		ret += 0.15 * assessDirection(cgs, nd, x + getXOff(nd), y + getYOff(nd), tl, recurse - 1);

		// Check right.
		nd = Direction (((d + 3) % 4) + 1);
		ret += 0.15 * assessDirection(cgs, nd, x + getXOff(nd), y + getYOff(nd), tl, recurse - 1);
	}

	// TODO Add a heuristic which keeps us from straying too far from our territory.

	return ret;
}
