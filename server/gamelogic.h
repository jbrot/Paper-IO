/**
 * gamelogic.h
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Provides the entry method to run the game logic invoked by the backend.
 */

#ifndef GAMELOGIC_H 
#define GAMELOGIC_H

#include <vector>

#include "GameState.h"

/*
 * Find the requested number of spawn points and return
 * them in a vector in the form (x,y). The spawn points
 * need to be relatively valid---that is, if three locations
 * are returned, three players need to be able to be spawned
 * immediately with their locations being the three returned
 * values. If there are not enough spots available, it is
 * okay to return fewer locations than requested (even none!)
 * and then fewer players will be spawned.
 */
std::vector<std::pair<pos_t, pos_t> > findSpawns(int num, GameState &state);

/*
 * Configures the spawn area for the given player. The player
 * will already have been added to the game at a location
 * provided by findSpawns, so their location can be queried with
 * getX() and getY(). This should set up the initial territory.
 */
void configureSpawn(Player *pl, GameState &state);

void updateGame(GameState &state);

#endif // !GAMELOGIC_H
