/*
 * Implements GameState
 */

#include "gamestate.h"

GameState::GameState(pos_t w, pos_t h)
	: width(w)
	, height(h)
	, players()
	, tick(0)
	, oobs(OUT_OF_BOUNDS_STATE)
	, oobd(0)
	, oobf(0)
{
	// Create the board array
	board = new state_t *[height];
	board[0] = new state_t[width * height];
	std::fill(board[0], board[0] + (width * height), 0);
	for (int i = 1; i < height; i++)
		board[i] = board[i - 1] + width;

	// Create the diff array
	diff = new state_t *[height];
	diff[0] = new state_t[width * height];
	std::fill(diff[0], diff[0] + (width * height), 0);
	for (int i = 1; i < height; i++)
		diff[i] = diff[i - 1] + width;

	// Create the flags array
	flags = new quint8 *[height + 2];
	flags[0] = new quint8[(width + 2) * (height + 2)];
	std::fill(flags[0], flags[0] + ((width + 2) * (height + 2)), 0);
	for (int i = 1; i < height + 2; i++)
		flags[i] = flags[i - 1] + width + 2;
}

GameState::~GameState()
{
	foreach(Player *pl, players)
		delete pl;

	delete[] board[0];
	delete[] board;

	delete[] diff[0];
	delete[] diff;

	delete[] flags[0];
	delete[] flags;
}

pos_t GameState::getWidth() const
{
	return width;
}

pos_t GameState::getHeight() const
{
	return height;
}

tick_t GameState::getTick() const
{
	return tick;
}

void GameState::nextTick()
{
	tick++;
}

SquareState GameState::getState(pos_t x, pos_t y)
{
	if (0 <= x && x < width && 0 <= y && y < height)
		return SquareState(*this, x, y, board[y][x], diff[y][x], flags[y + 1][x + 1]);

	quint8 *flag = NULL;
	if (0 <= x + 1 && x + 1 < width + 2 && 0 <= y + 1 && y + 1 <= height + 2)
		flag = &flags[y + 1][x + 1];
	else
		flag = &oobf;

	return SquareState(*this, x, y, oobs, oobd, oobf);
}

Player *GameState::lookupPlayer(plid_t id) const
{
	if (!players.contains(id))
		return NULL;
	return players.value(id);
}

std::vector<Player> GameState::getPlayers() const
{
	std::vector<Player> pls;
	foreach (Player *pl, players)
	{
		pls.push_back(*pl);
	}

	return pls;
}

bool GameState::addPlayer(plid_t id, pos_t x, pos_t y)
{
	SquareState ss = getState(x,y);
	if (ss.isOccupied() || players.contains(id))
		return false;

	ss.setOccupyingPlayerId(id);
	ss.setDirection(Direction::NONE);

	players.insert(id, new Player(*this, id, x, y));

	return true;
}

void GameState::removePlayer(plid_t id)
{
	if (!players.contains(id))
	{
		qWarning() << "Tried to remove non-existant player" << id << "!";
		return;
	}

	Player *pl = players.value(id);
	players.remove(id);

	if (!pl)
	{
		qWarning() << "Player" << id << "is NULL!";
		return;
	}

	// If the player is still on the board, remove them.
	SquareState ss = getState(pl->getX(), pl->getY());
	if (ss.getOccupyingPlayerId() == id)
	{
		ss.setOccupyingPlayerId(UNOCCUPIED);
		ss.setDirection(Direction::NONE);
	}

	delete pl;
}
