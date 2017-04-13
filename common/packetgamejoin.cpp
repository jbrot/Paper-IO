/*
 * Game Join packet. Informs the client it is now in game and provides
 * initial state information. N.B., the current tick will be sent three
 * times, but this isn't a big deal and simplifies the net code. Only
 * the first tick value will actually be used.
 *
 * Spec: <PACKET_GAME_JOIN> <quint8: id> <quint8: score>
 *       <contents of PACKET_PLAYERS_UPDATE>
 *       <contents of PACKET_LEADERBOARD_UPDATE>
 *       <contents of PACKET_RESEND_BOARD>
 * Direction: Server to Client
 */

#include "protocol.h"

PacketGameJoin::PacketGameJoin()
	: Packet(PACKET_GAME_JOIN)
	, plid(NULL_ID)
	, score(0)
	, total(0)
	, ppu()
	, plu()
	, prb()
{
}

PacketGameJoin::PacketGameJoin(plid_t id, score_t sc, quint16 ts, quint16 tr, const PacketPlayersUpdate &ppuc, const PacketLeaderboardUpdate &pluc, const PacketResendBoard &prbc)
	: Packet(PACKET_GAME_JOIN)
	, plid(id)
	, score(sc)
	, total(ts)
	, tickRate(tr)
	, ppu(ppuc)
	, plu(pluc)
	, prb(prbc)
{
}

plid_t PacketGameJoin::getId() const
{
	return plid;
}

void PacketGameJoin::setId(plid_t id)
{
	plid = id;
}

score_t PacketGameJoin::getScore() const
{
	return score;
}

void PacketGameJoin::setScore(score_t sc)
{
	score = sc;
}

quint16 PacketGameJoin::getTotalSquares() const
{
	return total;
}

void PacketGameJoin::setTotalSquares(quint16 ts)
{
	total = ts;
}

quint16 PacketGameJoin::getTickRate() const
{
	return tickRate;
}

void PacketGameJoin::setTickRate(quint16 tr)
{
	tickRate = tr;
}

const PacketPlayersUpdate &PacketGameJoin::getPPU() const
{
	return ppu;
}

void PacketGameJoin::setPPU(const PacketPlayersUpdate &ppuc)
{
	ppu = ppuc;
}

const PacketLeaderboardUpdate &PacketGameJoin::getPLU() const
{
	return plu;
}

void PacketGameJoin::setPLU(const PacketLeaderboardUpdate &pluc)
{
	plu = pluc;
}

const PacketResendBoard &PacketGameJoin::getPRB() const
{
	return prb;
}

void PacketGameJoin::setPRB(const PacketResendBoard &prbc)
{
	prb = prbc;
}

void PacketGameJoin::read(QDataStream &str)
{
	str >> plid >> score >> total >> tickRate >> ppu >> plu >> prb;
}

void PacketGameJoin::write(QDataStream &str) const
{
	str << plid << score << total << tickRate << ppu << plu << prb;
}
