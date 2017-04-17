/*
 * This file contains the implmentation of AIPlayer. It contains
 * all of the actual AI logic.
 */

#include "aiplayer.h"

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

AIPlayer::AIPlayer(plid_t pid)
	: id(pid)
	, traillen(0)
{
}

Direction AIPlayer::tick(const GameState &cgs)
{
	const Player *pl = cgs.lookupPlayer(id);
	if (!pl)
	{
		qCritical() << "Player object is NULL!";
		return Direction::NONE;
	}

	Direction d = pl->getActualDirection();
	if (d == Direction::NONE)
		return Direction::NONE;

	if (pl->getState().getOwningPlayerId() == pl->getId())
		traillen = 0;
	else
		++traillen;

	std::fill(heur[0], heur[0] + CLIENT_FRAME * CLIENT_FRAME, -1);

	double straight = assessDirection(cgs, d, pl->getX() + getXOff(d), pl->getY() + getYOff(d), traillen, 6);

	Direction ld = Direction((d % 4) + 1);
	double left = assessDirection(cgs, ld, pl->getX() + getXOff(ld), pl->getY() + getYOff(ld), traillen, 6);

	Direction rd = Direction(((d + 2) % 4) + 1);
	double right = assessDirection(cgs, rd, pl->getX() + getXOff(rd), pl->getY() + getYOff(rd), traillen, 6);

	if (straight >= left && straight >= right)
		return d;
	else if (left >= straight && left >= right)
		return ld;
	else
		return rd;
}

double AIPlayer::assessDirection(const GameState &cgs, Direction d, pos_t x, pos_t y, int tl, int recurse)
{
	SquareState state = cgs.getState(x, y);
	// Penalize dangerous actions
	if (state.isOutOfBounds())
		return 0;
	if (state.getTrailPlayerId() == id) 
		return 0;
	if (state.isOccupied() && !state.isOutOfBounds() && state.getOccupyingPlayerId() != id)
		return 0;

	int dist = std::max(computeDistance(cgs, x, y), 0);
	double ret = 10 - exp(dist / 8); 

	// Assess the trail probability. This is either a good or bad
	// thing depending on how aggressibe we are.
	if (state.hasTrail() && state.getTrailPlayerId() != id)
		ret += 1000;

	// We like to complete trails of length 10, so return a value
	// which gets large quickly around 10 and then tapers off.
	if (state.getOwningPlayerId() == id) 
	{
		// Double our aggressiveness twoards players in our territory.
		ret *= 2;

		if (tl > 0)
		{
			const int FALLOFF = 10;
			double xp = tl > FALLOFF ? tl - FALLOFF : 4 * (tl - FALLOFF);
			if (traillen > 1.5 * FALLOFF)
				xp = tl - traillen;
			double coef = exp(-(xp * xp));
			return ret + coef * 1000;
		}
	} else {
		++tl;

		// Incentive to leave our territory.
		ret += exp(-(tl - 2) * (tl - 2)) * 750;
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
		nd = Direction (((d + 2) % 4) + 1);
		ret += 0.15 * assessDirection(cgs, nd, x + getXOff(nd), y + getYOff(nd), tl, recurse - 1);
	}

	return ret;
}

struct BFSE {
	int x;
	int y;
};

int AIPlayer::computeDistance(const GameState &cgs, pos_t x, pos_t y)
{
	// We've already checked it's not null.
	const Player *pl = cgs.lookupPlayer(id);
	int ax = x - pl->getX() + CLIENT_FRAME / 2;
	int ay = y - pl->getY() + CLIENT_FRAME / 2;

	if (heur[ay][ax] != -1)
		return heur[ay][ax];

	if (cgs.getState(x, y).getOwningPlayerId() == id)
		return (heur[ay][ax] = 0);

	QVector<BFSE> searched;
	QVector<BFSE> terr;
	QVector<BFSE> searching;
	int ub = 15;
	int dist = 0;
	searched.push_back({ax, ay});
	while (++dist <= ub)
	{
		for (int i = -dist; i <= dist; ++i)
		{
			int j = dist - abs(i);
			if (j == 0)
			{
				searching.push_back({ax + i, ay});
			} else {
				searching.push_back({ax + i, ay + j});
				searching.push_back({ax + i, ay - j});
			}
		}

		bool found = true;
		while (!searching.isEmpty())
		{
			BFSE bf = searching.last();
			searching.removeLast();
			if (bf.x < 0 || bf.x >= CLIENT_FRAME || bf.y < 0 || bf.y >= CLIENT_FRAME)
				continue;

			if (heur[bf.y][bf.x] == -2)
				continue;

			if (heur[bf.y][bf.x] != -1)
			{
				terr.push_back(bf);
				if (heur[bf.y][bf.x] + dist < ub)
					ub = heur[bf.y][bf.x] + dist;
				continue;
			}

			const SquareState ss = cgs.getState(bf.x - ax + pl->getX(), bf.y - ay + pl->getY());
			if (ss.getOwningPlayerId() == id)
			{
				heur[bf.y][bf.x] = 0;
				ub = dist;
				terr.push_back(bf);
				continue;
			}

			searched.push_back(bf);
			found = false;
		}

		if (found)
			break;
	}

	for (auto iter = searched.cbegin(); iter < searched.cend(); ++iter)
	{
		BFSE s = *iter;
		int bdist = abs(s.x - ax) + abs(s.y - ay);
		int dist = -1;
		for (auto titer = terr.cbegin(); titer < terr.cend(); ++titer)
		{
			BFSE t = *titer;
			int tdist = abs(t.x - ax) + abs(t.y - ay);
			int rdist = abs(t.x - s.x) + abs(t.y - s.y);
			if (rdist <= tdist - bdist)
				dist = dist == -1 ? rdist + heur[t.y][t.x] : std::min(rdist + heur[t.y][t.x], dist);
		}
		if (dist != -1)
			heur[s.y][s.x] = dist;
	}

	if (heur[ay][ax] == -1)
		heur[ay][ax] = -2;

	return heur[ay][ax];
}
