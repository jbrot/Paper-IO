/*
 * Players Update packet. Informs the client that the currently connected players have changed.
 *
 * Spec: <PACKET_PLAYERS_UPDATE> <tick_t: current tick> <QHash<plid_t, QString>: id/player map>
 * Direction: Server to Client
 */

#include "protocol.h"

PacketPlayersUpdate::PacketPlayersUpdate()
	: Packet(PACKET_PLAYERS_UPDATE)
	, tick(0)
	, players()
{
}

PacketPlayersUpdate::PacketPlayersUpdate(tick_t tck, const QHash<plid_t, QString> &pls)
	: Packet(PACKET_PLAYERS_UPDATE)
	, tick(tck)
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
	str >> tick >> players;
}

void PacketPlayersUpdate::write(QDataStream &str) const
{
	str << tick << players;
}

