/*
 * This is a clone of the AI software which runs on the client while in
 * kiosk mode. In a perfect world the code would be reused...
 */
#ifndef KIOSKAI_H
#define KIOSKAI_H

#include "clientgamestate.h"

class KioskAI
{
public:
	KioskAI();

	Direction tick(const ClientGameState &gs);
private:
	int traillen;

	double assessDirection(const ClientGameState &pgs, Direction d, pos_t x, pos_t y, int traillen, int recurse = 5);
};

#endif // !KIOSKAI_H
