/*
 * This class does the actual square state prediction of the AI.
 */

#include "aiplayer.h"

static bool areOpposites(Direction a, Direction b)
{
	return a != 0 && b != 0 && abs(a - b) == 2;
}

PredictedSquareState::PredictedSquareState(const SquareState &s, const PredictedGameState *p)
	: ss(s)
	, parent(p)
	, head()
	, trail()
	//, owned()
{
	// If this isn't a prediction, just lift the info.
	if (!parent)
	{
		if (s.getOccupyingPlayer())
			head.insert(qMakePair(s.getOccupyingPlayerId(), s.getOccupyingPlayer()->getActualDirection()), 1);

		if (s.getTrailPlayer())
			trail.insert(s.getTrailPlayerId(), 1);
		return;
	}

	// First, calculate the occupying chance. We use the following
	// heuristic: a player has an 80% chance of going straight, a 10%
	// chance of turning left and a 10% chance of turning right.
	processHead(p->getPrediction(getX(), getY() - 1), UP);
	processHead(p->getPrediction(getX(), getY() + 1), DOWN);
	processHead(p->getPrediction(getX() - 1, getY()), LEFT);
	processHead(p->getPrediction(getX() + 1, getY()), RIGHT);

	// And now we have to do inclusion exclusion to ensure the probabilities
	// are mutually exclusive. We will go by directions which keeps this
	// computation from getting unreasonably large.
	for (Direction a = Direction(1); a < 4; a = Direction(a + 1))
	{
		for (Direction b = Direction(a + 1); b <= 4; b = Direction(b + 1))
		{
			for (auto aiter = head.begin(); aiter != head.end(); ++aiter)
			{
				if (aiter.key().second != a)
					continue;
				for (auto biter = aiter + 1; biter != head.end(); ++biter)
				{
					if (biter.key().second != b)
						continue;

					if (ss.getOwningPlayerId() == aiter.key().first)
					{
						*biter -= (*aiter) * (*biter);
					} else if (ss.getOwningPlayerId() == biter.key().first) {
						*aiter -= (*aiter) * (*biter);
					} else {
						*aiter -= 0.5 * (*aiter) * (*biter);
						*biter -= 0.5 * (*aiter) * (*biter);
					}
				}
			}
		}
	}

	// The three and four case will basically never happen and if they do, I guess
	// we'll have slightly off probabilities. Probably not that big of a deal.
	
	// Calculate the trail probabilities from the previous iteration's head probabilities.
	const PredictedSquareState &pt = p->getPrediction(getX(), getY());
	for (auto iter = pt.head.begin(); iter != pt.head.end(); ++iter)
		trail[iter.key().first] += *iter;
}

void PredictedSquareState::processHead(const PredictedSquareState &ps, Direction d)
{
	Direction enter = Direction(((d + 1) % 4) + 1);
	for (auto iter = ps.head.begin(); iter != ps.head.end(); ++iter)
	{
		// If they are moving away from us, they have no chance of moving
		// into this square.
		if (iter.key().second == d)
			continue;

		// If they are moving toward us, we give them an 80% chance of continuing
		if (areOpposites(iter.key().second, d))
			head[qMakePair(iter.key().first, enter)] = 0.8 * iter.value();
		// Otherwise we put their odds at 10%
		else
			head[qMakePair(iter.key().first, enter)] = 0.1 * iter.value();
	}
}

pos_t PredictedSquareState::getX() const
{
	return ss.getX();
}

pos_t PredictedSquareState::getY() const
{
	return ss.getY();
}

prob_t PredictedSquareState::getHeadChance(plid_t exclude) const
{
	if (!parent)
		return ss.isOccupied() && ss.getOccupyingPlayerId() != exclude;

	double total = 0;
	for (auto iter = head.cbegin(); iter != head.cend(); ++iter)
		if (iter.key().first != exclude)
			total += iter.value();

	return total;
}

prob_t PredictedSquareState::getTrailChance(plid_t exclude) const
{
	if (!parent)
		return ss.hasTrail() && ss.getTrailPlayerId() != exclude;

	double total = 0;
	for (auto iter = trail.cbegin(); iter != trail.cend(); ++iter)
		if (iter.key() != exclude)
			total += iter.value();

	return total;
}

/*
prob_t PredictedSquareState::getOwnedChance(plid_t exclude) const
{
	if (!parent)
		return ss.isOwned() && ss.getOwningPlayerId() != exclude;

	double total = 0;
	for (auto iter = owned.cbegin(); iter != owned.cend(); ++iter)
		if (iter.key() != exclude)
			total += iter.value();

	return total;
}
*/
