/*
 * Game Tick packet. Informs the client of a server tick and change in board state.
 *
 * Spec: <PACKET_GAME_TICK> <tick_t: current tick> <quint8: direction_moved> <quint8: score>
 *       {<quint32: board_state>}[CLIENT_FRAME times, the new row visible either L to R or T to B depending on direction]
 *       {<quint8>}[RLE encoded XOR difference of existing board, L to R, T to B]
 *       <quint64: a 64 bit crc of the new board state>
 * Direction: Server to Client
 */

#include "protocol.h"

PacketGameTick::PacketGameTick()
	: Packet(PACKET_GAME_TICK)
	, tick(0)
	, dir(0)
	, score(0)
	, alloc(true)
	, chksum(0)
{
	std::fill(news, news + CLIENT_FRAME, 0);
	diff[0] = new state_t[CLIENT_FRAME * CLIENT_FRAME];
	for (int i = 1; i < CLIENT_FRAME; i++)
		diff[i] = diff[0] + i * CLIENT_FRAME;
}

PacketGameTick::PacketGameTick(tick_t tck, Direction dr, quint8 sc, const state_t ns[CLIENT_FRAME], state_t *brd[CLIENT_FRAME], const QByteArray &chk)
	: Packet(PACKET_GAME_TICK)
	, tick(tck)
	, dir(dr)
	, score(sc)
	, alloc(false)
	, chksum(chk)
{
	std::copy(ns, ns + CLIENT_FRAME, news);
	std::copy(brd, brd + CLIENT_FRAME, diff);
}

PacketGameTick::~PacketGameTick()
{
	if (alloc)
		delete[] diff[0];
}

tick_t PacketGameTick::getTick() const
{
	return tick;
}

void PacketGameTick::setTick(tick_t tck)
{
	tick = tck;
}

Direction PacketGameTick::getDirection() const
{
	return Direction(dir);
}

void PacketGameTick::setDirection(Direction d)
{
	dir = d;
}

quint8 PacketGameTick::getScore() const
{
	return score;
}

void PacketGameTick::setScore(quint8 sc)
{
	score = sc;
}

const state_t *PacketGameTick::getNewSection() const
{
	return news;
}

void PacketGameTick::setNewSection(const state_t ns[CLIENT_FRAME])
{
	std::copy(ns, ns + CLIENT_FRAME, news);
}

const state_t *const *PacketGameTick::getDiff() const
{
	return diff;
}

void PacketGameTick::setDiffPointer(state_t *brd[CLIENT_FRAME], const QByteArray &chk)
{
	if (alloc)
		delete[] diff[0];
	alloc = false;

	std::copy(brd, brd + CLIENT_FRAME, diff);
	chksum = chk;
}

void PacketGameTick::setDiffCopy(state_t const *brd[CLIENT_FRAME], const QByteArray &chk)
{
	if (!alloc)
	{
		diff[0] = new state_t[CLIENT_FRAME * CLIENT_FRAME];
		for (int i = 1; i < CLIENT_FRAME; i++)
			diff[i] = diff[0] + i * CLIENT_FRAME;
	}
	alloc = true;

	for (int i = 0; i < CLIENT_FRAME; i++)
		std::copy(brd[i], brd[i] + CLIENT_FRAME, diff[i]);
	chksum = chk;
}

QByteArray PacketGameTick::getChecksum() const
{
	return chksum;
}

/*
 * The diffs are RLE encoded. This means we write a byte
 * indicating "quantity" and then a quint32 which will be
 * repeated "quantity" times.
 */
void PacketGameTick::read(QDataStream &str)
{
	str >> tick >> dir >> score;
	for (int i = 0; i < CLIENT_FRAME; i++)
		str >> news[i];

	quint8 count = 0;
	state_t cv = 0;
	for (int i = 0; i < CLIENT_FRAME; i++)
	{
		for (int j = 0; j < CLIENT_FRAME; j++)
		{
			if (count == 0)
				str >> count >> cv;

			diff[i][j] = cv;
			count--;
		}
	}

	str >> chksum;
}

void PacketGameTick::write(QDataStream &str) const
{
	str << tick << dir << score;
	for (int i = 0; i < CLIENT_FRAME; i++)
		str << news[i];

	quint8 count = 0;
	state_t cv = diff[0][0];
	for (int i = 0; i < CLIENT_FRAME; i++)
	{
		for (int j = 0; j < CLIENT_FRAME; j++)
		{
			if (diff[i][j] == cv)
			{
				count++;
				if (count < count + 1)
					continue;
			}

			str << count << cv;
			count = 1;
			cv = diff[i][j];
		}
	}
	str << count << cv;

	str << chksum;
}
