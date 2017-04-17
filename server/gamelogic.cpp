/*
 * gamelogic.cpp
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Holds the game logic which occurs on each tick
 */

#include <QtCore>

#include "gamelogic.h"

void updatePosition(Player& player, GameState &state);
void leaveTrail(Player &player, GameState &state);
void killPlayers(GameState &state);
void checkForTrail(Player &player, GameState &state);
void floodMarkSquares(Player &player, GameState &state);
void fillInBody(Player &player, GameState &state);
void checkForCompletedLoop(Player &player, GameState &state);
bool detectWin(Player &player, GameState &state);
bool squareChecks(Player &player, SquareState square, GameState &state);
Direction calculateDirection(Player &player, GameState &state);

std::vector<std::pair<pos_t, pos_t> > findSpawns(int num, GameState &state);
bool checkIfSpawnable(pos_t xPos, pos_t yPos, GameState &state);

void configureSpawn(Player *pl, GameState &state);

void updateGame(GameState &state)
{

	// Create vector of all Players
	std::vector<Player *> allPlayers = state.getPlayers();

	// Loop over all Players
	for (int i = 0; i < allPlayers.size(); ++i)
	{
		if (!allPlayers[i])
		{
			qWarning() << "Null player at" << i << "!";
			continue;
		}

		// Leave trail under player
		leaveTrail(*allPlayers[i], state);

		// Update position of player
		updatePosition(*allPlayers[i], state);

		// Check if player hit a trail
		checkForTrail(*allPlayers[i], state);

		// Check if player completed a loop
		checkForCompletedLoop(*allPlayers[i], state);

		// Check for winner
		if (detectWin(*allPlayers[i], state))
			allPlayers[i]->kill();

	}
	
	// Kill dead players
	killPlayers(state);

}

void updatePosition(Player &player, GameState &state)
{
	Direction newD = calculateDirection(player, state);

	bool res = true;
	pos_t newX = player.getX();
	pos_t newY = player.getY();
	switch (newD){
	case UP:
		newY -= 1;
		break;
	case RIGHT:
		newX += 1;
		break;
	case DOWN:
		newY += 1;
		break;
	case LEFT:
		newX -= 1;
		break;
	case NONE:
		player.setActualDirection(NONE);
		return;
	}

	res = player.setLocation(newX, newY);

	// res is false if there is a boundary or a player collision
	if (!res)
	{
		SquareState ss = state.getState(newX, newY);
		if (ss.getOccupyingPlayer() && ss.getOwningPlayerId() == player.getId())
		{
			ss.getOccupyingPlayer()->kill();
			player.setLocation(newX, newY);
		} else {
			player.kill();
		}
	}

	player.setActualDirection(newD);
}

void leaveTrail(Player &player, GameState &state)
{
	pos_t xpos = player.getX();
	pos_t ypos = player.getY();

	Direction newD = calculateDirection(player, state);
	Direction old = player.getActualDirection();

	SquareState square = state.getState(xpos, ypos);

	if (square.getOwningPlayerId() == player.getId())
		return;

	square.setTrailPlayer(&player);

	if (old == newD)
	{
		if (newD == UP || newD == DOWN)
			square.setTrailType(NORTHTOSOUTH);
		else if (newD == LEFT || newD == RIGHT)
			square.setTrailType(EASTTOWEST);
	}
	else
	{
		if (old == UP && newD == RIGHT)
			square.setTrailType(NORTHTOEAST);
		if (old == UP && newD == LEFT)
			square.setTrailType(NORTHTOWEST);
		if (old == RIGHT && newD == DOWN)
			square.setTrailType(NORTHTOWEST);
		if (old == RIGHT && newD == UP)
			square.setTrailType(SOUTHTOWEST);
		if (old == DOWN && newD == RIGHT)
			square.setTrailType(SOUTHTOEAST);
		if (old == DOWN && newD == LEFT)
			square.setTrailType(SOUTHTOWEST);
		if (old == LEFT && newD == UP)
			square.setTrailType(SOUTHTOEAST);
		if (old == LEFT && newD == DOWN)
			square.setTrailType(NORTHTOEAST);
	}

}

Direction calculateDirection(Player &player, GameState &state)
{
	Direction newD = player.getNewDirection();
	Direction old = player.getActualDirection();

	if (abs(newD - old) == 2 && old != NONE)
		newD = old;

	if (state.getTick() - player.getSpawnTick() <= 4)
		newD = NONE;
	else if (newD == NONE)
	{
		if (old != NONE)
			newD = old;
		else
			newD = Direction((rand() % 4) + 1);
	}

	return newD;
}

void killPlayers(GameState &state)
{
	// Loop over all squares
	for (int j = 0; j <= (state.getWidth() - 1); ++j){
		for (int k = 0; k <= (state.getHeight() - 1); ++k){

			SquareState square = state.getState(j, k);

			// Check if square is has a trail and kill accordingly
			if (square.getTrailPlayer() && square.getTrailPlayer()->isDead()){
				square.setTrailType(NOTRAIL);
				square.setTrailPlayerId(UNOCCUPIED);
			}

			// Check if square is owned and kill accordingly
			if (square.getOwningPlayer() && square.getOwningPlayer()->isDead()){
				square.setOwningPlayerId(UNOCCUPIED);
			}
		}
	}
}

void checkForTrail(Player &player, GameState &state)
{
	pos_t xpos = player.getX();
	pos_t ypos = player.getY();
	
	SquareState square = state.getState(xpos, ypos);
	
	if (square.hasTrail()){
		square.getTrailPlayer()->kill();
	}
}

void floodMarkSquares(Player &player, GameState &state)
{
	std::vector<std::pair<pos_t, pos_t>> coordinatesStack;
	coordinatesStack.push_back({-1,-1});
	state.getState(-1,-1).markAsChecked();

	while (!coordinatesStack.empty())
	{
		auto last = coordinatesStack.back();

		SquareState thisSquare = state.getState(last.first, last.second);
		SquareState up = state.getState(last.first, last.second - 1);
		SquareState down = state.getState(last.first, last.second + 1);
		SquareState left = state.getState(last.first - 1, last.second);
		SquareState right = state.getState(last.first + 1, last.second);

		if (squareChecks(player, up, state))
		{
			up.markAsChecked();
			coordinatesStack.push_back({up.getX(),up.getY()});
			continue;
		}
		up.markAsChecked();

		if (squareChecks(player, down, state))
		{
			down.markAsChecked();
			coordinatesStack.push_back({down.getX(),down.getY()});
			continue;
		}
		down.markAsChecked();

		if (squareChecks(player, left, state))
		{
			left.markAsChecked();
			coordinatesStack.push_back({left.getX(),left.getY()});
			continue;
		}
		left.markAsChecked();

		if (squareChecks(player, right, state))
		{
			right.markAsChecked();
			coordinatesStack.push_back({right.getX(),right.getY()});
			continue;
		}
		right.markAsChecked();

		thisSquare.markAsFlooded();
		coordinatesStack.pop_back();
	}

}

bool squareChecks(Player &player, SquareState square, GameState &state)
{
	if (square.getX() < -1 || square.getY() < -1 ||
		square.getX() > state.getWidth() || square.getY() > state.getHeight())
		return false;
	if (square.hasBeenChecked())
		return false;
	if (square.getOwningPlayerId() == player.getId())
		return false;
	if (square.getTrailPlayerId() == player.getId())
		return false;

	return true;

}

void fillInBody(Player &player, GameState &state)
{

	for (int i = -1; i <= state.getWidth(); ++i)
	{
		for (int j = -1; j <= state.getHeight(); ++j)
		{

			SquareState square = state.getState(i, j);
			if (!square.isFlooded())
			{
				if (square.getTrailPlayerId() == player.getId())
				{
					square.setTrailType(TrailType::NOTRAIL);
					square.setTrailPlayerId(UNOCCUPIED);
				}

				if (square.getOwningPlayerId() != player.getId())
				{
					if (square.isOwned())
						square.getOwningPlayer()->setScore(square.getOwningPlayer()->getScore() - 1);

					square.setOwningPlayerId(player.getId());
					player.setScore(player.getScore() + 1);
				}
			}
			square.markAsUnflooded();
			square.markAsUnchecked();

		}
	}
}

void checkForCompletedLoop(Player &player, GameState &state)
{
	pos_t xpos = player.getX();
	pos_t ypos = player.getY();

	bool trailExists = false;

	SquareState square = state.getState(xpos, ypos);

	for (int i = xpos - 1; i <= xpos + 1; ++i)
	{
		for (int j = ypos - 1; j <= ypos + 1; ++j)
		{
			if (state.getState(i, j).getTrailPlayerId() == player.getId())
			{
				trailExists = true;
				break;
			}
		}
	}

	if (square.getOwningPlayerId() == player.getId() && trailExists)
	{
		qDebug() << "Filling in stuff.";
		floodMarkSquares(player, state);
		fillInBody(player, state);
	}

}

bool detectWin(Player &player, GameState &state){

	for (int i = 0; i <= state.getWidth() -1 ; ++i)
	{
		for (int j = 0; j <= state.getHeight() - 1; ++j)
		{
			if (state.getState(i, j).getOwningPlayerId() != player.getId())
				return false;
		}
	}

	return true;
}



std::vector<std::pair<pos_t, pos_t> > findSpawns(int num, GameState &state)
{

	std::vector<std::pair<pos_t, pos_t>> availableSpawns;
	std::vector<std::pair<pos_t, pos_t>> randomSpawns;

	for (int i = 2; i <= state.getWidth() - 3; i = i + 5)
	{
		for (int j = 2; j <= state.getHeight() - 3; j = j + 5)
		{
			if (checkIfSpawnable(i, j, state))
				availableSpawns.push_back({i,j});
		}
	}

	std::random_shuffle(availableSpawns.begin(), availableSpawns.end());

	for (int m = 0; m < availableSpawns.size() && m < num; ++m)
	{
		randomSpawns.push_back(availableSpawns[m]);
	}

	return randomSpawns;
}

bool checkIfSpawnable(pos_t xPos, pos_t yPos, GameState &state){

	for (int i = xPos - 2; i <= xPos + 2; ++i)
	{
		for (int j = yPos - 2; j <= yPos + 2; ++j)
		{
			if (state.getState(i, j).hasTrail() || state.getState(i, j).isOccupied() || state.getState(i, j).isOwned())
				return false;
		}
	}

	return true;
}



void configureSpawn(Player *pl, GameState &state)
{

	pos_t xPos = pl->getX();
	pos_t yPos = pl->getY();

	for (int i = xPos - 1; i <= xPos + 1; ++i)
	{
		for (int j = yPos - 1; j <= yPos + 1; ++j)
		{
			state.getState(i, j).setOwningPlayer(pl);
			pl->setScore(pl->getScore() + 1);
		}
	}

}
