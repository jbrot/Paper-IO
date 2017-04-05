/*
 * This file provides implementations for Packet.
 */

#include <QCryptographicHash>
#include <QtCore>

#include "protocol.h"

QByteArray hashBoard(state_t const* const* board)
{
	QCryptographicHash hash(QCryptographicHash::Algorithm::Md4);
	for (int i = 0; i < CLIENT_FRAME; i++)
		hash.addData(reinterpret_cast<const char *>(board[i]), CLIENT_FRAME * sizeof(state_t) / sizeof(char));
	return hash.result();
}

std::unordered_map<packet_t, std::unique_ptr<APacketFactory>> Packet::map;

void Packet::registerPacket(packet_t id, std::unique_ptr<APacketFactory> fact)
{
	Packet::map[id] = std::move(fact);
}

Packet::Packet(packet_t pid)
	: id(pid)
{
}

Packet::~Packet()
{
}

packet_t Packet::getId() const
{
	return id;
}

// The default read and writes for a packet are empty.
void Packet::read(QDataStream &str)
{
}

void Packet::write(QDataStream &str) const
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

void Packet::writePacket(QDataStream &str, const Packet &packet)
{
	str << packet.getId() << packet;
	qDebug() << "Sent packet:" << packet.getId();
}

Packet *Packet::readPacket(QDataStream &str)
{
	// This is relatively involved, so we don't want to deal
	// with a broken stream.
	if (str.status()) return NULL;

	packet_t id;
	str >> id;
	if (str.status()) return NULL;

	qDebug() << "Received packet header:" << id;

	auto factory = Packet::map.find(id);
	if (factory == Packet::map.end())
	{
		qWarning() << "Tried to read in unregistered packet:" << id;
		return NULL;
	} else if (!factory->second) {
		qWarning() << "Tried to read in improperly registered packet:" << id;
		return NULL;
	}

	Packet *obj = factory->second->instantiate();
	if (!obj)
	{
		qWarning() << "Could not allocate packet:" << id;
		return NULL;
	}

	obj->read(str);
	if (str.status())
	{
		qDebug() << "Read in failed:" << str.status();
		// If reading in failed, clean up after ourselves.
		delete obj;
		return NULL;
	}
	qDebug() << "Read packet:" << id;
	return obj;
}
