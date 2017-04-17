/*
 * This class represents an AI Player in the game.
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "gamestate.h"
#include "protocol.h"

class AIPlayer
{
public:
	AIPlayer(plid_t player);

	Direction tick(const GameState &gs);
private:
	const plid_t id;
	int traillen;
	int heur[CLIENT_FRAME][CLIENT_FRAME];

	double assessDirection(const GameState &pgs, Direction d, pos_t x, pos_t y, int traillen, int recurse = 5);
	int computeDistance(const GameState &cgs, pos_t x, pos_t y);
};

#endif // !AIPLAYER_H
