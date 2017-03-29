/*
 * End Game packet. Informs the player that their game has ended and provides the final score. Note
 * this does not distinguish between death and victory. The client may either quit or issue a
 * PACKET_REQUEST_JOIN to continue playing.
 *
 * Spec: <PACKET_GAME_END> <quint8: score>
 * quint8: Serber to Client
 */

#include "protocol.h"

PacketGameEnd::PacketGameEnd()
	: score(0)
{
}

PacketGameEnd::PacketGameEnd(quint8 nm)
	: score(nm)
{
}

quint8 PacketGameEnd::getScore() const
{
	return quint8(score);
}

void PacketGameEnd::setScore(quint8 str)
{
	score = str;
}

void PacketGameEnd::read(QDataStream &str)
{
	str >> score;
}

void PacketGameEnd::write(QDataStream &str) const
{
	str << score;
}

