/*
 * Leaderboard Update packet. Informs the client that the leaderboard has changed.
 * 
 * Spec: <PACKET_LEADERBOARD_UPDATE> <tick_t: current tick> {<quint8: player_id> <quint8: score>}[5 times: leader board in descending order]
 * Direction: Server to Client
 */

#include "protocol.h"

PacketLeaderboardUpdate::PacketLeaderboardUpdate()
	: Packet(PACKET_LEADERBOARD_UPDATE)
	, tick(0)
{
	std::fill(data, data + 10, 0);
}

PacketLeaderboardUpdate::PacketLeaderboardUpdate(tick_t tck, const quint8 lb[10])
	: Packet(PACKET_LEADERBOARD_UPDATE)
	, tick(tck)
{
	std::copy(lb, lb + 10, data);
}

tick_t PacketLeaderboardUpdate::getTick() const
{
	return tick;
}

void PacketLeaderboardUpdate::setTick(tick_t tck)
{
	tick = tck;
}

const quint8 *PacketLeaderboardUpdate::getLeaderboard() const
{
	return data;
}

void PacketLeaderboardUpdate::setLeaderboard(const quint8 lb[10])
{
	std::copy(lb, lb + 10, data);
}

void PacketLeaderboardUpdate::read(QDataStream &str)
{
	str >> tick;
	for (int i = 0; i < 10; i++)
		str >> data[i];
}

void PacketLeaderboardUpdate::write(QDataStream &str) const
{
	str << tick;
	for (int i = 0; i < 10; i++)
		str << data[i];
}
