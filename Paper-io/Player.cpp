/**
* Player.h
*
* EECS 183, Winter 2017
* Final Project: Paper-io
*
* Represents a player
*/

#include "Player.h"
using namespace std;

Player::Player(string nickname, Point Spawn, int number, Color shade) {

    setPosition(Spawn);
    setName(nickname);
    setID(number);
    setColor(shade);

}

void Player::setName(string nickname) {
    name = nickname;
}

void Player::setPosition(Point pt) {
    position = pt;
}

void Player::setID(int identity) {
    ID = identity;
}

void Player::setColor(Color newColor) {
    color = newColor;
}

void Player::setDirection(int input) {
    direction = input;
}

string Player::getName() {
    return name;
}

Point Player::getPosition() {
    return position;
}

int Player::getID() {
    return ID;
}

Color Player::getColor() {
    return color;
}

int Player::getDirection() {
    return direction;
}