/*
 * This file provides implementations for Packet.
 */

#include <QCryptographicHash>

#include "protocol.h"

QByteArray hashBoard(state_t const* const* board)
{
	QCryptographicHash hash(QCryptographicHash::Algorithm::Md4);
	for (int i = 0; i < CLIENT_FRAME; i++)
		hash.addData(reinterpret_cast<const char *>(board[i]), CLIENT_FRAME * sizeof(state_t) / sizeof(char));
	return hash.result();
}

Packet::~Packet()
{
}

QDataStream &operator<<(QDataStream &str, const Packet &packet)
{
	packet.write(str);
	return str;
}

QDataStream &operator>>(QDataStream &str, Packet &packet)
{
	packet.read(str);
	return str;
}

// The default read and writes for a packet are empty.
void Packet::read(QDataStream &str)
{
}

void Packet::write(QDataStream &str) const
{
}
