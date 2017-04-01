/**
 * player.cpp
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Represents a player
 */

#include "gamestate.h"

Player::Player(const GameState &ngs, const plid_t pid, pos_t cx, pos_t cy)
	: gs(ngs)
	, id(pid)
	, x(cx)
	, y(cy)
	, newDir(Direction::NONE)
	, dir(Direction::NONE)
	, dead(false)
	, winner(false)
{
}

plid_t Player::getId() const
{
	return id;
}

pos_t Player::getX() const
{
	return x;
}

bool Player::setX(pos_t newX)
{
	return setLocation(y, newX);
}

pos_t Player::getY() const
{
	return y;
}

bool Player::setY(pos_t newY)
{
	return setLocation(newY, x);
}

bool Player::setLocation(pos_t newX, pos_t newY)
{
	SquareState ns = gs.getState(x, y);
	if (ns.isOccupied())
		return false;

	// We need to update the state to reflect our new location.
	SquareState ss = gs.getState(x, y);
	ss.setOccupyingPlayerId(UNOCCUPIED);
	ss.setDirection(Direction::NONE);

	x = newX;
	y = newY;

	ns.setOccupyingPlayerId(id);
	ns.setDirection(dir);

	return true;
}

Direction Player::getNewDirection() const
{
    return newDir;
}

Direction Player::getActualDirection() const
{
    return dir;
}

void Player::setActualDirection(Direction nd)
{
    dir = nd;

	SquareState ss = gs.getState(x, y);
	ss.setDirection(dir);
}

bool Player::isDead() const
{
	return dead;
}

void Player::setDead(bool state)
{
	dead = state;
}

bool Player::isWinner() const
{
	return winner;
}

void Player::setWinner()
{
	winner = true;
}
