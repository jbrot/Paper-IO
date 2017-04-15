/*
 * This class does the actual square state prediction of the AI.
 */

#include "aiplayer.h"

PredictedSquareState::PredictedSquareState(const SquareState &s, const PredictedGameState *p)
	: ss(s)
	, parent(p)
	, head()
	, trail()
	, owned()
{
	if (!parent)
		return;

	// TODO compute prediction
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
		if (iter.key() != exclude)
			total += iter.value().second;

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
