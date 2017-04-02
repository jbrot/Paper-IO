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
void floodMarkSquares(Player player, GameState &state, pos_t xpos, pos_t ypos);
void fillInBody(Player player, GameState &state);
void checkForCompletedLoop(Player player, GameState &state);
bool detectWin(Player player, GameState &state);

void updateGame(GameState &state)
{

	if (!state.getPlayers())
	{
		qCritical() << "ERROR: Players vector is NULL";
		return;
	}

	// Create vector of all Players
	std::vector<Player> allPlayers = *state.getPlayers();

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

		// Check if player wins
		if (detectWin(allPlayers[i], state))
			allPlayers[i].setWinner();

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

	player.setOldDirection(newD);

}

void leaveTrail(Player &player, GameState &state)
{
	pos_t xpos = player.getX();
	pos_t ypos = player.getY();

	Direction old = player.getOldDirection();
	Direction newD = player.getNewDirection();

	SquareState square = state.getState(xpos, ypos);

	square.setOccupyingPlayer(&player);

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
			    square.setOccupyingPlayerId(UNOCCUPIED);
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
	bool dead = true;
	
	SquareState square = state.getState(xpos, ypos);
	
	if (square.getTrailType() != 0){
		square.getOccupyingPlayer()->setDead(dead);
	}
}

void checkForBoundary(Player player, GameState &state)
{

	pos_t xpos = player.getX();
	pos_t ypos = player.getY();
	bool dead = true;

	if (xpos < 1 || xpos > state.getWidth())
		player.setDead(dead);
	if (ypos < 1 || ypos > state.getHeight())
		player.setDead(dead);
}

void floodMarkSquares(Player player, GameState &state, pos_t xpos, pos_t ypos)
{

	// Note: I have NO IDEA what I'm doing in this function
	// God have mercy upon this code
	// This is meant to recursively check adjacent squares
	// A square which is reachable is marked as "flooded"

	// Fix square
	SquareState square = state.getState(xpos, ypos);

	// Check if already checked
	if (square.hasBeenChecked())
		return;

	// Mark square as checked
	square.markAsChecked();

	// Check if out of bounds
	if (xpos < 0 || xpos > (state.getWidth() + 1) || ypos < 0 || ypos > (state.getWidth() + 1))
			return;

	// Detect if square is occupied or owned by player
	if (square.getOccupyingPlayerId() == player.getId() || square.getOwningPlayerId() == player.getId())
		return;

	// If not, mark it as flooded
	square.markAsFlooded();

	// Move on to adjacent squares
	floodMarkSquares(player, state, xpos + 1, ypos);
	floodMarkSquares(player, state, xpos - 1, ypos);
	floodMarkSquares(player, state, xpos, ypos + 1);
	floodMarkSquares(player, state, xpos, ypos - 1);

	// Pray to heavenly Jesus that this works
}

void fillInBody(Player player, GameState &state)
{

	SquareState square = state.getState(0, 0);

	for (int i = 0; i <= (state.getWidth() + 1); ++i)
	{
		for (int j = 0; j <= (state.getHeight() + 1); ++i)
		{

			square = state.getState(i, j);
			if (!square.isFlooded())
				square.setOwningPlayerId(player.getId());

			square.markAsUnflooded();
			square.markAsUnchecked();

		}
	}
}

void checkForCompletedLoop(Player player, GameState &state)
{
	plid_t xpos = player.getX();
	plid_t ypos = player.getY();

	SquareState square = state.getState(xpos, ypos);

	if (square.getOwningPlayerId() == player.getId())
	{
		floodMarkSquares(player, state, 0, 0);
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
