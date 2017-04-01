/**
* Player.cpp
*
* EECS 183, Winter 2017
* Final Project: Paper-io
*
* Represents a player
*/

#include "Player.h"

plid_t Player::getId() {
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

void Player::setNewDirection(Direction newD){
    newDir = newD;
}

Direction Player::getOldDirection(){
    return oldDir;
}

void Player::setOldDirection(Direction old){
    oldDir = old;
}

bool Player::isDead() {
	return dead;
}

void Player::setDead(bool state) {
	dead = state;
}
