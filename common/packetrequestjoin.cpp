/*
 * Request Join packet. Indicates the client wants to join
 * a new game. The subsequent string sets the player's name.
 * If the string is empty, then the player's name is unchanged.
 * If the string is empty initially, then a default name will
 * be assigned.
 *
 * Spec: <PACKET_REQUEST_JOIN> <QString: name> 
 * Direction: Client to Server
 */

#include "protocol.h"

PacketRequestJoin::PacketRequestJoin()
	: name()
{
}

PacketRequestJoin::PacketRequestJoin(const QString &nm)
	: name(nm)
{
}

QString PacketRequestJoin::getName() const
{
	return name;
}

void PacketRequestJoin::setName(const QString &str)
{
	name = str;
}

void PacketRequestJoin::read(QDataStream &str)
{
	str >> name;
}

void PacketRequestJoin::write(QDataStream &str) const
{
	str << name;
}

