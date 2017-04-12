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
	: Packet(PACKET_GAME_END)
	, score(0)
{
}

PacketGameEnd::PacketGameEnd(score_t nm)
	: Packet(PACKET_GAME_END)
	, score(nm)
{
}

score_t PacketGameEnd::getScore() const
{
	return score;
}

void PacketGameEnd::setScore(score_t str)
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

