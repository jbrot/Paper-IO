/*
 * player.cpp
 *
 * EECS 183, Winter 2017
 * Final Project: Paper-io
 *
 * Represents a player
 */

#include "gamestate.h"

Player::Player(GameState &ngs, const plid_t pid, const QString &nm, pos_t cx, pos_t cy)
	: gs(ngs)
	, id(pid)
	, name(nm)
	, spawn(ngs.getTick())
	, x(cx)
	, y(cy)
	, newDir(Direction::NONE)
	, score(0)
	, dead(false)
{
}

plid_t Player::getId() const
{
	return id;
}

QString Player::getName() const
{
	return name;
}

pos_t Player::getX() const
{
	return x;
}

bool Player::setX(pos_t newX)
{
	return setLocation(newX, y);
}

pos_t Player::getY() const
{
	return y;
}

bool Player::setY(pos_t newY)
{
	return setLocation(x, newY);
}

bool Player::setLocation(pos_t newX, pos_t newY)
{
	SquareState ns = gs.getState(newX, newY);
	if (ns.isOccupied())
		return false;

	// We need to update the state to reflect our new location.
	SquareState ss = gs.getState(x, y);
	Direction dir = ss.getDirection();
	ss.setOccupyingPlayerId(UNOCCUPIED);
	ss.setDirection(Direction::NONE);

	x = newX;
	y = newY;

	ns.setOccupyingPlayerId(id);
	ns.setDirection(dir);

	return true;
}

SquareState Player::getState()
{
	return gs.getState(x, y);
}

const SquareState Player::getState() const
{
	return static_cast<const GameState &>(gs).getState(x, y);
}

Direction Player::getNewDirection() const
{
	return newDir;
}

Direction Player::getActualDirection() const
{
	SquareState ss = gs.getState(x, y);
	return ss.getDirection();
}

void Player::setActualDirection(Direction nd)
{
	SquareState ss = gs.getState(x, y);
	ss.setDirection(nd);
}

bool Player::isDead() const
{
	return dead;
}

void Player::kill()
{
	dead = true;

	SquareState ss = gs.getState(x, y);
	ss.setOccupyingPlayerId(UNOCCUPIED);
	ss.setDirection(Direction::NONE);

	// We don't move to -1,-1 because that square
	// may be used during territory checking.
	x = -5;
	y = -5;
}

score_t Player::getScore() const
{
	return score;
}

void Player::setScore(score_t sc)
{
	if (sc == score)
		return;

	score = sc;
	gs.markScoresChanged();
}

tick_t Player::getSpawnTick() const
{
	return spawn;
}
