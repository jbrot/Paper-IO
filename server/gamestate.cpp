/*
 * Implements GameState
 */

#include "gamestate.h"
#include "protocol.h"

const int EXTRA_BUFFER = CLIENT_FRAME / 2;

GameState::GameState(pos_t w, pos_t h)
	: width(w)
	, height(h)
	, lock()
	, players()
	, playersChanged(false)
	, tick(0)
	, scoresChanged(false)
	, leaderboardChanged(false)
{
	std::fill(leaderboard, leaderboard + 5, std::make_pair(NULL_ID, 0));

	// The board and diff array start with a CLIENT_FRAME length section
	// of out of bounds, which clients will link to if an entire row is
	// out of range. Then they consist of each row of the board padded
	// by EXTRA_BUFFER of out of bounds. This EXTRA_BUFFER allows the clients
	// to read directly from the array when they are slightly off the screen.

	// Create the board array
	boardStart = new state_t[CLIENT_FRAME + (width + EXTRA_BUFFER) * height];
	std::fill(boardStart, boardStart + CLIENT_FRAME, OUT_OF_BOUNDS_STATE);

	board = new state_t *[height];
	board[0] = boardStart + CLIENT_FRAME;
	std::fill(board[0], board[0] + width, 0);
	std::fill(board[0] + width, board[0] + width + EXTRA_BUFFER, OUT_OF_BOUNDS_STATE);
	for (int i = 1; i < height; i++)
	{
		board[i] = board[i - 1] + width + EXTRA_BUFFER;
		std::fill(board[i], board[i] + width, 0);
		std::fill(board[i] + width, board[i] + width + EXTRA_BUFFER, OUT_OF_BOUNDS_STATE);
	}

	// Create the diff array
	diffStart = new state_t[CLIENT_FRAME + (width + EXTRA_BUFFER) * height];
	std::fill(diffStart, diffStart + (width + EXTRA_BUFFER) * height, 0);

	diff = new state_t *[height];
	diff[0] = diffStart + CLIENT_FRAME;
	for (int i = 1; i < height; i++)
		diff[i] = diff[i - 1] + width + EXTRA_BUFFER;

	// Create the flags array
	// There is one extra flag at the end to use for invalid square states.
	flags = new quint8 *[height + 2];
	flags[0] = new quint8[(width + 2) * (height + 2) + 1];
	std::fill(flags[0], flags[0] + ((width + 2) * (height + 2)) + 1, 0);
	for (int i = 1; i < height + 2; i++)
		flags[i] = flags[i - 1] + width + 2;
}

GameState::~GameState()
{
	foreach(Player *pl, players)
		delete pl;

	delete[] boardStart;
	delete[] board;

	delete[] diffStart;
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

	// Reset the diffs. Note we don't need to touch the initial
	// CLIENT_STATE buffer as it can never change.
	std::fill(diff[0], diff[0] + (width + EXTRA_BUFFER) * height, 0);

	playersChanged = false;
	scoresChanged = false;
	leaderboardChanged = false;
}

SquareState GameState::getState(pos_t x, pos_t y) const
{
	if (0 <= x && x < width && 0 <= y && y < height)
		return SquareState(*this, x, y, board[y][x], diff[y][x], flags[y + 1][x + 1]);

	quint8 *flag = NULL;
	if (0 <= x + 1 && x + 1 < width + 2 && 0 <= y + 1 && y + 1 < height + 2)
		flag = &flags[y + 1][x + 1];
	else
		flag = &flags[height + 1][width + 2];

	return SquareState(*this, x, y, *boardStart, *diffStart, *flag);
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
		if (pl)
			pls.push_back(*pl);
	}

	return pls;
}

bool GameState::addPlayer(plid_t id, const QString &name, pos_t x, pos_t y)
{
	SquareState ss = getState(x,y);
	if (ss.isOccupied() || players.contains(id))
		return false;

	ss.setOccupyingPlayerId(id);
	ss.setDirection(Direction::NONE);

	players.insert(id, new Player(*this, id, name, x, y));

	playersChanged = true;

	return true;
}

QHash<plid_t, Player *>::iterator GameState::removePlayer(QHash<plid_t, Player *>::iterator i)
{
	Player *pl = i.value();
	auto ret = players.erase(i);

	if (!pl)
	{
		qWarning() << "Player" << i.key() << "is NULL!";
		return ret;
	}

	// If the player is still on the board, remove them.
	SquareState ss = getState(pl->getX(), pl->getY());
	if (ss.getOccupyingPlayerId() == i.key())
	{
		ss.setOccupyingPlayerId(UNOCCUPIED);
		ss.setDirection(Direction::NONE);
	}

	delete pl;

	playersChanged = true;

	return ret;
}

bool GameState::havePlayersChanged() const
{
	return playersChanged;
}

void GameState::markScoresChanged()
{
	scoresChanged = true;
}

bool GameState::haveScoresChanged() const
{
	return scoresChanged;
}

void GameState::recomputeLeaderboard()
{
	QList<Player *> pls = players.values();
	std::sort(pls.begin(), pls.end(), [] (Player *a, Player *b) -> bool {
		if (!a && !b)
			return false;
		if (!a)
			return false;
		if (!b)
			return true;

		return a->getScore() > b->getScore();
	});
	int i = 0;
	for (auto iter = pls.cbegin(); iter < pls.cend() && i < 5; ++iter, ++i)
	{
		if (!leaderboardChanged)
		{
			if (*iter) 
				leaderboardChanged = !(leaderboard[i].first == (*iter)->getId() && leaderboard[i].second == (*iter)->getScore());
			else 
				leaderboardChanged = !(leaderboard[i].first == NULL_ID && leaderboard[i].second == 0);
		} 
		if (*iter)
		{
			leaderboard[i].first = (*iter)->getId();
			leaderboard[i].second = (*iter)->getScore();
		} else {
			leaderboard[i].first = NULL_ID;
			leaderboard[i + 1].second = 0;
		}
	}

	for (; i < 5; ++i)
		leaderboard[i] = {NULL_ID, 0};
}

bool GameState::hasLeaderboardChanged() const
{
	return leaderboardChanged;
}

void GameState::lockForRead()
{
	lock.lockForRead();
}

void GameState::lockForWrite()
{
	lock.lockForWrite();
}

void GameState::unlock()
{
	lock.unlock();
}
