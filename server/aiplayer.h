/*
 * This class represents an AI Player in the game.
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "gamestate.h"

class AIPlayer
{
public:
	AIPlayer();

	Direction tick(const GameState &gs);
};

#endif // !AIPLAYER_H
