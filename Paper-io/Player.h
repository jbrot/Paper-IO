/**
 * Player.h
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Represents a player
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
using namespace std;

#include "Color.h"
#include "Point.h"

class Player
{
public:

    /**
     * Requires: Nothing.
     * Modifies: ID, color, position, name
     * Effects: Spawns a new player. Should be able to choose an ID,
     *          a unique color, and a valid spawn position.
     */
    Player(string name, Point Spawn, int number, Color shade);

    /**
     * Requires: Nothing.
     * Modifies: position.
     * Effects: Moves player to new position.
     */
    void setPosition(Point pt);

    /**
     * Requires: Nothing.
     * Modifies: Nothing.
     * Effects: Returns the player's position
     */
    Point getPosition();

    /**
     * Requires: Nothing.
     * Modifies: ID.
     * Effects: Sets player ID
     */
    void setID(int identity);

    /**
     * Requires: Nothing.
     * Modifies: Nothing.
     * Effects: Returns player ID
     */
    int getID();

    /**
     * Requires: Nothing.
     * Modifies: color.
     * Effects: Sets player color
     */
    void setColor(Color newColor);

    /**
     * Requires: Nothing.
     * Modifies: Nothing.
     * Effects: Returns player color.
     */
    Color getColor();

    /**
     * Requires: Nothing.
     * Modifies: name.
     * Effects: Sets name of player
     */
    void setName(string name);

    /**
     * Requires: Nothing.
     * Modifies: Nothing.
     * Effects: Returns player name.
     */
    string getName();

    void setDirection(int input);

    int getDirection();

private:
    string name;
    int ID;
    Color color;
    Point position;
    int direction;

};
#endif // !PLAYER_H
