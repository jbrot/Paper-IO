/**
* gamelogic.cpp
*
* EECS 183, Winter 2017
* Final Project: Paper-io
*
* Holds the game logic which occurs on each tick
*/


#include "gamelogic.h"
#include "Player.h"

void updatePosition(Player& player);
void leaveTrail(Player player, GameState &state);

void updateGame(GameState &state){

    // Create vector of all Players
    std::vector<Player> allPlayers = getPlayers();

    // Loop over all Players
    for (int i =0; i < allPlayers.size(); ++1){

        // Leave trail under player
        leaveTrail(allPlayers[i], state);

        // Update position of player
        updatePosition(allPlayers[i]);



    }
}

void updatePosition(Player& player){

    Direction newD = player.getNewDirection();

    switch (newD){
    case UP:
        player.setY(player.getY - 1);
        break;
    case RIGHT:
        player.setX(player.getX + 1);
        break;
    case DOWN:
        player.setY(player.getY + 1);
        break;
    case LEFT:
        player.setX(player.getX - 1);
        break;
    }

    player.setOldDirection(newD);

}

void leaveTrail(Player player, GameState &state){

    pos_t xpos = player.getX();
    pos_t ypos = player.getY();

    Direction old = player.getOldDirection();
    Direction newD = player.getNewDirection();

    SquareState square = state.getState(xpos, ypos);

    square.setOccupyingPlayer(player.getId());

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
