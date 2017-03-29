/*
 * Players Update packet. Informs the client that the currently connected players have changed.
 *
 * Spec: <PACKET_PLAYERS_UPDATE> <tick_t: current tick> <QHash<plid_t, QString>: id/player map>
 * Direction: Server to Client
 */

#include "protocol.h"

PacketPlayersUpdate::PacketPlayersUpdate()
	: tick(0)
	, players()
{
}

PacketPlayersUpdate::PacketPlayersUpdate(tick_t tck, const QHash<plid_t, QString> &pls)
	: tick(tck)
	, players(pls)
{
}

tick_t PacketPlayersUpdate::getTick() const
{
	return tick;
}

void PacketPlayersUpdate::setTick(tick_t tck)
{
	tick = tck;
}

QHash<plid_t, QString> PacketPlayersUpdate::getPlayers() const
{
	return players;
}

void PacketPlayersUpdate::setPlayers(const QHash<plid_t, QString> &pls)
{
	players = pls;
}

void PacketPlayersUpdate::read(QDataStream &str)
{
	str >> players;
}

void PacketPlayersUpdate::write(QDataStream &str) const
{
	str << players;
}

