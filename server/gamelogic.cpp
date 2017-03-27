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
#include "Player.h"

void updatePosition(Player& player);
void leaveTrail(Player &player, GameState &state);
void killPlayers(GameState &state);
void checkForTrail(Player player, GameState &state);

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
    for (int i =0; i < allPlayers.size(); ++i)
    {

        // Leave trail under player
        leaveTrail(allPlayers[i], state);

        // Update position of player
        updatePosition(allPlayers[i]);

        // Check if player hit a trail
        checkForTrail(allPlayers[i], state);

    }

    // Kill dead players
    killPlayers(state);

}

void updatePosition(Player& player){

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
    }

    player.setOldDirection(newD);


}

void leaveTrail(Player &player, GameState &state){

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

void killPlayers(GameState &state){

    // Loop over all squares
    for (int j = 0; j < state.getWidth(); ++j){
        for (int k = 0; k < state.getHeight(); ++k){

            SquareState square = state.getState(j, k);

            // Check if square is occupied and kill accordingly
            if (square.getOccupyingPlayer() && square.getOccupyingPlayer()->isDead()){
                square.setTrailType(NOTRAIL);
                square.setOccupyingPlayerId(NULL_ID);
            }

            // Check if square is owned and kill accordingly
            if (square.getOwningPlayer() && square.getOwningPlayer()->isDead()){
                square.setOwningPlayer(NULL_ID);
            }
        }

    }
}

void checkForTrail(Player player, GameState &state){

    pos_t xpos = player.getX();
    pos_t ypos = player.getY();

    SquareState square = state.getState(xpos, ypos);

    if (square.getTrailType() != 0){
        square.getOccupyingPlayer()->isDead();
    }

}
