/*
 * This class manage PredictedSquareStates, and allows for probabilistic extrapolation
 * from the current board state.
 */

#include "aiplayer.h"

PredictedGameState::PredictedGameState(const GameState &g, const PredictedGameState *p)
	: gs(g)
	, parent(p)
	, child(nullptr)
{
	// Dynamic memory, woo!!!
	board = new PredictedSquareState **[gs.getHeight()];
	board[0] = new PredictedSquareState *[gs.getWidth() * gs.getHeight() + 1];
	std::fill(board[0], board[0] + gs.getWidth() * gs.getHeight() + 1, nullptr);
	for (int i = 1; i < gs.getHeight(); ++i)
		board[i] = board[i - 1] + gs.getWidth();
}

PredictedGameState::~PredictedGameState()
{
	if (child)
		delete child;

	for (int i = 0; i < gs.getHeight(); ++i)
		for (int j = 0; j < gs.getWidth(); ++j)
			if (board[i][j])
				delete board[i][j];
	delete[] board[0];
	delete[] board;
}

const GameState &PredictedGameState::getGS() const
{
	return gs;
}

const PredictedSquareState &PredictedGameState::getPrediction(pos_t x, pos_t y) const
{
	PredictedSquareState *ss;
	if (0 <= x && x < gs.getWidth() && 0 <= y && y < gs.getHeight())
		ss = board[y][x];
	else
		ss = board[gs.getHeight() - 1][gs.getWidth()];

	if (!ss)
		ss = new PredictedSquareState(gs.getState(x, y), parent);
	return *ss;
}

PredictedGameState &PredictedGameState::getNextState()
{
	if (!child)
		child = new PredictedGameState(gs, this);
	return *child;
}
