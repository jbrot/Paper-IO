/*
 * Leaderboard Update packet. Informs the client that the leaderboard has changed.
 * 
 * Spec: <PACKET_LEADERBOARD_UPDATE> <tick_t: current tick> {<plid_t: player_id> <score_t: score>}[5 times: leader board in descending order]
 * Direction: Server to Client
 */

#include "protocol.h"

PacketLeaderboardUpdate::PacketLeaderboardUpdate()
	: Packet(PACKET_LEADERBOARD_UPDATE)
	, tick(0)
{
	std::fill(data, data + 5, std::make_pair(NULL_ID, 0));
}

PacketLeaderboardUpdate::PacketLeaderboardUpdate(tick_t tck, const std::pair<plid_t, score_t> lb[5])
	: Packet(PACKET_LEADERBOARD_UPDATE)
	, tick(tck)
{
	std::copy(lb, lb + 5, data);
}

tick_t PacketLeaderboardUpdate::getTick() const
{
	return tick;
}

void PacketLeaderboardUpdate::setTick(tick_t tck)
{
	tick = tck;
}

const std::pair<plid_t, score_t> *PacketLeaderboardUpdate::getLeaderboard() const
{
	return data;
}

void PacketLeaderboardUpdate::setLeaderboard(const std::pair<plid_t, score_t> lb[10])
{
	std::copy(lb, lb + 5, data);
}

void PacketLeaderboardUpdate::read(QDataStream &str)
{
	str >> tick;
	for (int i = 0; i < 5; i++)
		str >> data[i].first >> data[i].second;
}

void PacketLeaderboardUpdate::write(QDataStream &str) const
{
	str << tick;
	for (int i = 0; i < 5; i++)
		str << data[i].first << data[i].second;
}
