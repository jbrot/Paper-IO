/**
* Player.cpp
*
* EECS 183, Winter 2017
* Final Project: Paper-io
*
* Represents a player
*/

#include "Player.h"

id_t Player::getId() {
	return id;
}

std::string Player::getName() {
	return name;
}

void Player::setX(pos_t newX) {
	x = newX;
}

pos_t Player::getX() {
	return x;
}

void Player::setY(pos_t newY) {
	y = newY;
}

pos_t Player::getY() {
	return y;
}

void Player::setLocation(pos_t newX, pos_t newY) {
	x = newX;
	y = newY;
}

Direction Player::getNewDirection(){
    return newDir;
}

Direction Player::getOldDirection(){
    return oldDir;
}

bool Player::isDead() {
	return !alive;
}

void Player::setDead(bool dead) {
	alive = dead;
}
