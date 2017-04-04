/*
 * Resend Board packet. Sends the entire local board state after a client request.
 *
 * Spec: <PACKET_RESEND_BOARD> <tick_t: current tick> {<state_t: board state>}[CLIENT_FRAME^2 times, populating the local board state, L to R, T to B] <quint64: 64 bit crc>
 * Direction: Server to Client
 */

#include "protocol.h"

PacketResendBoard::PacketResendBoard()
	: Packet(PACKET_RESEND_BOARD)
	, tick(0)
	, alloc(true)
{
	allocBoard();

	chksum = hashBoard(board);
}

PacketResendBoard::PacketResendBoard(tick_t tck, state_t *brd[CLIENT_FRAME])
	: Packet(PACKET_RESEND_BOARD)
	, tick(tck)
	, alloc(false)
{
	std::copy(brd, brd + CLIENT_FRAME, board);

	chksum = hashBoard(board);
}

PacketResendBoard::PacketResendBoard(const PacketResendBoard &other)
	: Packet(PACKET_RESEND_BOARD)
	, tick(other.tick)
	, alloc(other.alloc)
	, chksum(other.chksum)
{
	if (alloc)
	{
		allocBoard();
		std::copy(other.board[0], other.board[0] + (CLIENT_FRAME * CLIENT_FRAME), board[0]);
	} else {
		std::copy(other.board, other.board + CLIENT_FRAME, board);
	}
}

PacketResendBoard::~PacketResendBoard()
{
	if (alloc)
		delete[] board[0];
}

void PacketResendBoard::allocBoard()
{
	board[0] = new state_t[CLIENT_FRAME * CLIENT_FRAME];
	for (int i = 1; i < CLIENT_FRAME; i++)
		board[i] = board[0] + i * CLIENT_FRAME;
}

tick_t PacketResendBoard::getTick() const
{
	return tick;
}

void PacketResendBoard::setTick(tick_t tck)
{
	tick = tck;
}

const state_t *const *PacketResendBoard::getBoard() const
{
	return board;
}

void PacketResendBoard::setBoardPointer(state_t *brd[CLIENT_FRAME])
{
	if (alloc)
		delete[] board[0];
	alloc = false;

	std::copy(brd, brd + CLIENT_FRAME, board);
	chksum = hashBoard(board);
}

void PacketResendBoard::setBoardCopy(state_t const *brd[CLIENT_FRAME])
{
	if (!alloc)
		allocBoard();
	alloc = true;

	for (int i = 0; i < CLIENT_FRAME; i++)
		std::copy(brd[i], brd[i] + CLIENT_FRAME, board[i]);
	chksum = hashBoard(board);
}

void PacketResendBoard::read(QDataStream &str)
{
	str >> tick;
	for (int i = 0; i < CLIENT_FRAME; i++)
		for (int j = 0; j < CLIENT_FRAME; j++)
			str >> board[i][j];
	str >> chksum;
}

void PacketResendBoard::write(QDataStream &str) const
{
	str << tick;
	for (int i = 0; i < CLIENT_FRAME; i++)
		for (int j = 0; j < CLIENT_FRAME; j++)
			str << board[i][j];
	str << chksum;
}

QByteArray PacketResendBoard::getChecksum() const
{
	return chksum;
}
