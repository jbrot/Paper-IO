/*
 * Update Direction packet. Informs the server the direction the client wants to move.
 * This may be ignored if invalid.
 *
 * Spec: <PACKET_UPDATE_DIR> <quint8: direction>
 * Direction: Client to Server
 */

#include "protocol.h"

PacketUpdateDir::PacketUpdateDir()
	: dir(Direction::NONE)
{
}

PacketUpdateDir::PacketUpdateDir(Direction nm)
	: dir(nm)
{
}

Direction PacketUpdateDir::getDirection() const
{
	return Direction(dir);
}

void PacketUpdateDir::setDirection(Direction str)
{
	dir = str;
}

void PacketUpdateDir::read(QDataStream &str)
{
	str >> dir;
}

void PacketUpdateDir::write(QDataStream &str) const
{
	str << dir;
}

