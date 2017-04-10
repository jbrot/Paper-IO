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

void updatePosition(Player& player);
void leaveTrail(Player &player, GameState &state);
void killPlayers(GameState &state);
void checkForTrail(Player player, GameState &state);
void checkForBoundary(Player player, GameState &state);
void floodMarkSquares(Player player, GameState &state);
void fillInBody(Player player, GameState &state);
void checkForCompletedLoop(Player player, GameState &state);
bool detectWin(Player player, GameState &state);

std::vector<std::pair<pos_t, pos_t> > findSpawns(int num, GameState &state);
bool checkIfSpawnable(pos_t xPos, pos_t yPos, GameState &state);

void configureSpawn(Player *pl, GameState &state);

void updateGame(GameState &state)
{

	// Create vector of all Players
	std::vector<Player> allPlayers = state.getPlayers();

	// Loop over all Players
	for (int i = 0; i < allPlayers.size(); ++i)
	{

		// Leave trail under player
		leaveTrail(allPlayers[i], state);

		// Update position of player
		updatePosition(allPlayers[i]);

		// Check if player hit a trail
		checkForTrail(allPlayers[i], state);

		// Check if player hit a boundary
		checkForBoundary(allPlayers[i], state);

		// Check if player completed a loop
		checkForCompletedLoop(allPlayers[i], state);

	}
	
	// Kill dead players
	killPlayers(state);

}

void updatePosition(Player& player)
{
	Direction newD = player.getNewDirection();

	switch (newD){
	case UP:
		player.setY(player.getY() - 1);
		break;
	case RIGHT:
		player.setX(player.getX() + 1);
		break;
	case DOWN:
		player.setY(player.getY() + 1);
		break;
	case LEFT:
		player.setX(player.getX() - 1);
		break;
	case NONE:
		break;
	}

	player.setActualDirection(newD);

}

void leaveTrail(Player &player, GameState &state)
{
	pos_t xpos = player.getX();
	pos_t ypos = player.getY();

	Direction old = player.getActualDirection();
	Direction newD = player.getNewDirection();

	SquareState square = state.getState(xpos, ypos);

	square.setTrailPlayer(&player);

	if (old == newD)
	{
		if (newD == 1 || newD == 3)
			square.setTrailType(NORTHTOSOUTH);
		else if (newD == 2 || newD == 4)
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

void killPlayers(GameState &state)
{
	// Loop over all squares
	for (int j = 0; j <= (state.getWidth() + 1); ++j){
		for (int k = 0; k <= (state.getHeight() + 1); ++k){

			SquareState square = state.getState(j, k);

			// Check if square is occupied and kill accordingly
			if (square.getOccupyingPlayer() && square.getOccupyingPlayer()->isDead()){
				square.setTrailType(NOTRAIL);
			}

			// Check if square is owned and kill accordingly
			if (square.getOwningPlayer() && square.getOwningPlayer()->isDead()){
				square.setOwningPlayerId(UNOCCUPIED);
			}
		}
	}
}

void checkForTrail(Player player, GameState &state)
{
	pos_t xpos = player.getX();
	pos_t ypos = player.getY();
	
	SquareState square = state.getState(xpos, ypos);
	
	if (square.getTrailType() != 0){
		square.getOccupyingPlayer()->kill();
	}
}

void checkForBoundary(Player player, GameState &state)
{

	pos_t xpos = player.getX();
	pos_t ypos = player.getY();

	if (xpos < 0 || xpos > state.getWidth() - 1)
		player.kill();
	if (ypos < 0 || ypos > state.getHeight() - 1)
		player.kill();
}

void floodMarkSquares(Player player, GameState &state)
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

		if (!(up.isOutOfBounds() || up.getOwningPlayerId() == player.getId()
			|| up.getTrailPlayerId() == player.getId() || up.hasBeenChecked()))
		{
			coordinatesStack.push_back({up.getX(),up.getY()});
			up.markAsChecked();
			continue;
		}
		if (!(down.isOutOfBounds() || player.getId() == down.getOwningPlayerId() || player.getId()
			== down.getTrailPlayerId() || down.hasBeenChecked()))
		{
			coordinatesStack.push_back({down.getX(),down.getY()});
			down.markAsChecked();
			continue;
		}
		if (!(left.isOutOfBounds() || player.getId() == left.getOwningPlayerId() || player.getId()
			== left.getTrailPlayerId() || left.hasBeenChecked()))
		{
			coordinatesStack.push_back({left.getX(),left.getY()});
			left.markAsChecked();
			continue;
		}
		if (!(right.isOutOfBounds() || player.getId() == right.getOwningPlayerId() || player.getId()
			== right.getTrailPlayerId() || right.hasBeenChecked()))
		{
			coordinatesStack.push_back({right.getX(),right.getY()});
			right.markAsChecked();
			continue;
		}

		thisSquare.markAsFlooded();
		coordinatesStack.pop_back();
	}

}

void fillInBody(Player player, GameState &state)
{

	for (int i = 0; i <= (state.getWidth() + 1); ++i)
	{
		for (int j = 0; j <= (state.getHeight() + 1); ++i)
		{

			SquareState square = state.getState(i, j);
			if (!square.isFlooded())
			{
				square.setOwningPlayerId(player.getId());

				if (square.getOwningPlayer())
					square.getOwningPlayer()->setScore(square.getOwningPlayer()->getScore() - 1);

				player.setScore(player.getScore() + 1);
			}

			square.markAsUnflooded();
			square.markAsUnchecked();

		}
	}
}

void checkForCompletedLoop(Player player, GameState &state)
{
	plid_t xpos = player.getX();
	plid_t ypos = player.getY();

	bool trailExists = false;

	SquareState square = state.getState(xpos, ypos);

	for (int i = xpos - 1; i <= xpos + 1; ++i)
	{
		for (int j = ypos - 1; j <= ypos + 1; ++j)
		{
			if (square.getTrailPlayer())
				trailExists = true;
		}
	}

	if (square.getOwningPlayerId() == player.getId() && trailExists)
	{
		floodMarkSquares(player, state);
		fillInBody(player, state);
	}

}

bool detectWin(Player player, GameState &state){

	SquareState square = state.getState(1, 1);

	for (int i = 1; i <= state.getWidth(); ++i)
	{
		for (int j = 1; j <= state.getHeight(); ++j)
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

	for (int i = 2; i <= state.getWidth() - 1; i = i + 3)
	{
		for (int j = 2; j <= state.getHeight() - 1; j = j + 3)
		{
			if (checkIfSpawnable(i, j, state))
				availableSpawns.push_back({i,j});
		}
	}

	for (int k = 0; k < availableSpawns.size() && k < num; ++k)
	{
		std::swap(availableSpawns[k], availableSpawns[k + (rand() % (availableSpawns.size() - k))]);
	}

	for (int m = 0; m < availableSpawns.size() && m < num; ++m)
	{
		randomSpawns.push_back(availableSpawns[m]);
	}

	return randomSpawns;
}

bool checkIfSpawnable(pos_t xPos, pos_t yPos, GameState &state){

	for (int i = xPos - 1; i <= xPos + 1; ++i)
	{
		for (int j = yPos - 1; j <= yPos + 1; ++j)
		{
			if (state.getState(i, j).hasTrail() || state.getState(i, j).isOccupied() || state.getState(i, j).isOwned())
				return false;
		}
	}

	return true;
}



void configureSpawn(Player *pl, GameState &state)
{

	int xPos = pl->getX();
	int yPos = pl->getY();

	for (int i = xPos - 1; i <= xPos + 1; ++i)
	{
		for (int j = yPos - 1; j <= yPos + 1; ++j)
		{
			state.getState(i, j).setOwningPlayer(pl);
			pl->setScore(pl->getScore() + 1);
		}
	}

}
