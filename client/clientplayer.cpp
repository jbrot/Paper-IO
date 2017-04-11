/*
 * Represents a player on the client.
 */

#include "clientgamestate.h"

ClientPlayer::ClientPlayer(const ClientGameState &ngs, const plid_t pid, const QString &nm, pos_t cx, pos_t cy)
	: gs(ngs)
	, id(pid)
	, name(nm)
	, x(cx)
	, y(cy)
	, score(0)
{
}

plid_t ClientPlayer::getId() const
{
	return id;
}

QString ClientPlayer::getName() const
{
	return name;
}

pos_t ClientPlayer::getX() const
{
	return x;
}

void ClientPlayer::setX(pos_t newX)
{
	x = newX;
}

pos_t ClientPlayer::getY() const
{
	return y;
}

void ClientPlayer::setY(pos_t newY)
{
	y = newY;
}

Direction ClientPlayer::getDirection() const
{
	ClientSquareState ss = gs.getState(x, y);
    return ss.getDirection();
}

score_t ClientPlayer::getScore() const
{
	return score;
}

void ClientPlayer::setScore(score_t sc)
{
	score = sc;
}
