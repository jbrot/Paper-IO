/*
 * This class represents an AI Player in the game.
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "gamestate.h"

typedef double prob_t;

class PredictedGameState;

class PredictedSquareState
{
public:
	pos_t getX() const;
	pos_t getY() const;
	const SquareState &getSS() const;

	/*
	 * Calculate the chances _exclusing_ the specified player.
	 */
	prob_t getHeadChance(plid_t exclude) const;
	prob_t getTrailChance(plid_t exclude) const;
	// I have no idea how to calculate this one.
	//prob_t getOwnedChance(plid_t exclude) const;

	PredictedSquareState(const SquareState &ss, const PredictedGameState *parent = nullptr);

private:
	const SquareState ss;
	const PredictedGameState *parent;
	QHash<QPair<plid_t, Direction>, prob_t> head;
	QHash<plid_t, prob_t> trail;
	//QHash<plid_t, prob_t> owned;

	void processHead(const PredictedSquareState &ps, Direction d);
};

class PredictedGameState
{
public:
	const GameState &getGS() const;

	const PredictedSquareState &getPrediction(pos_t x, pos_t y) const;

	PredictedGameState &getNextState();

	PredictedGameState(const GameState &gs, const PredictedGameState *parent = nullptr);
	~PredictedGameState();

	PredictedGameState(const PredictedGameState &other) = delete;

private:
	const GameState &gs;
	const PredictedGameState *parent;
	PredictedGameState *child;

	PredictedSquareState ***board;
};

class AIPlayer
{
public:
	AIPlayer(plid_t player);

	Direction tick(PredictedGameState &gs);
private:
	const plid_t id;
	int traillen;

	double assessDirection(PredictedGameState &pgs, Direction d, pos_t x, pos_t y, int traillen, int recurse = 5);
};

#endif // !AIPLAYER_H
