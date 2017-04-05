/*
 * Contains the complete specification for the client-server protocol.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <memory>
#include <QByteArray>
#include <QDataStream>
#include <QHash>
#include <unordered_map>

#include "types.h"

/* How long to wait for a keep alive packet before disconnecting */
const int TIMEOUT_LEN = 10;

/* How big of a view the client receives */
const int CLIENT_FRAME = 31;

typedef quint8 packet_t;

/* 
 * Keep Alive packet. 
 *
 * Spec: <PACKET_KEEP_ALIVE> 
 * Direction: Both ways
 */
const packet_t PACKET_KEEP_ALIVE = 0;
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
const packet_t PACKET_REQUEST_JOIN = 1;
/*
 * Queued packet. Informs the client it has been queued to join a
 * game (i.e., a PACKET_REQUEST_JOIN has been successfully processed).
 *
 * Spec: <PACKET_QUEUED>
 * Direction: Server to Client
 */
const packet_t PACKET_QUEUED = 2;
/*
 * Players Update packet. Informs the client that the currently connected players have changed.
 *
 * Spec: <PACKET_PLAYERS_UPDATE> <tick_t: current tick> <QHash<plid_t, QString>: id/player map>
 * Direction: Server to Client
 */
const packet_t PACKET_PLAYERS_UPDATE = 3;
/*
 * Leaderboard Update packet. Informs the client that the leaderboard has changed.
 * 
 * Spec: <PACKET_LEADERBOARD_UPDATE> <tick_t: current tick> {<quint8: player_id> <quint8: score>}[5 times: leader board in descending order]
 * Direction: Server to Client
 */
const packet_t PACKET_LEADERBOARD_UPDATE = 4;
/*
 * Resend Board packet. Sends the entire local board state after a client request.
 *
 * Spec: <PACKET_RESEND_BOARD> <tick_t: current tick> {<state_t: board state>}[CLIENT_FRAME^2 times, populating the local board state, L to R, T to B] <quint64: 64 bit crc>
 * Direction: Server to Client
 */
const packet_t PACKET_RESEND_BOARD = 5;
/*
 * Game Join packet. Informs the client it is now in game and provides
 * initial state information. N.B., the current tick will be sent three
 * times, but this isn't a big deal and simplifies the net code. Only
 * the first tick value will actually be used.
 *
 * Spec: <PACKET_GAME_JOIN> <plid_t: id> <quint8: score>
 *       <contents of PACKET_PLAYERS_UPDATE>
 *       <contents of PACKET_LEADERBOARD_UPDATE>
 *       <contents of PACKET_RESEND_BOARD>
 * Direction: Server to Client
 */
const packet_t PACKET_GAME_JOIN = 6;
/*
 * Game Tick packet. Informs the client of a server tick and change in board state.
 *
 * Spec: <PACKET_GAME_TICK> <tick_t: current tick> <quint8: direction_moved> <quint8: score>
 *       {<quint32: board_state>}[CLIENT_FRAME times, the new row visible either L to R or T to B depending on direction]
 *       {<quint8>}[RLE encoded XOR difference of existing board, L to R, T to B]
 *       <quint64: a 64 bit crc of the new board state>
 * Direction: Server to Client
 */
const packet_t PACKET_GAME_TICK = 7;
/*
 * Update Direction packet. Informs the server the direction the client wants to move.
 * This may be ignored if invalid.
 *
 * Spec: <PACKET_UPDATE_DIR> <quint8: direction>
 * Direction: Client to Server
 */
const packet_t PACKET_UPDATE_DIR = 8;
/*
 * Resend Request packet. Requests the server resend the local board state in its entirety (probably
 * because of a hash mismatch).
 *
 * Spec: <PACKET_REQUEST_RESEND>
 * Direction: Client to Server
 */
const packet_t PACKET_REQUEST_RESEND = 9;
/*
 * End Game packet. Informs the player that their game has ended and provides the final score. Note
 * this does not distinguish between death and victory. The client may either quit or issue a
 * PACKET_REQUEST_JOIN to continue playing.
 *
 * Spec: <PACKET_GAME_END> <quint8: score>
 * Direction: Serber to Client
 */
const packet_t PACKET_GAME_END = 10;

/*
 * Computes an md4 hash of the linked board for
 * client/server verification.
 */
QByteArray hashBoard(state_t const* const* board);

class Packet;

/*
 * The PacketFactory classes are used for dynamic allocation of packets to streamline IO handling.
 * They should be registered with Packet in the main function.
 */
class APacketFactory
{
public:
	virtual Packet *instantiate() = 0;
};

template<class T>
class PacketFactory : public APacketFactory
{
	static_assert(std::is_base_of<Packet, T>::value, "T must derive from Packet!");
public:

	Packet *instantiate() override
	{
		return new T();
	}
};

class Packet
{
public:
	/*
	 * The stream operators into Packet objects read/write the contents
	 * of a packet (i.e., not the header byte) into/from an already allocated
	 * Packet object.
	 */
	friend QDataStream &operator>>(QDataStream &str, Packet &packet);
	friend QDataStream &operator<<(QDataStream &str, const Packet &packet);

	static Packet *readPacket(QDataStream &str);
	static void writePacket(QDataStream &str, const Packet &packet);

	Packet(packet_t id);
	virtual ~Packet() = 0;

	packet_t getId() const;

	/*
	 * This registers a packet type to be used with the pointer stream
	 * read in operator.
	 *
	 * WARNING: The internal map is not locked, so when this method is called
	 * it must be guaranteed that no other threads will try to read from it
	 * (e.g., by trying to read in a packet). As a result, it is recommended
	 * that all packets are registered in the main function before any IO
	 * occurs.
	 */
	static void registerPacket(packet_t id, std::unique_ptr<APacketFactory> fact);

protected:
	virtual void read(QDataStream &str);
	virtual void write(QDataStream &str) const;

private:
	packet_t id;
	static std::unordered_map<packet_t, std::unique_ptr<APacketFactory>> map;
};

class PacketKeepAlive : public Packet
{
public:
	PacketKeepAlive()
		: Packet(PACKET_KEEP_ALIVE)
	{
	}
};

class PacketRequestJoin : public Packet
{
public:
	PacketRequestJoin();
	PacketRequestJoin(const QString &str);

	QString getName() const;
	void setName(const QString &name);

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	QString name;
};

class PacketQueued : public Packet
{
public:
	PacketQueued()
		: Packet(PACKET_QUEUED)
	{
	}
};

class PacketPlayersUpdate : public Packet
{
public:
	PacketPlayersUpdate();
	PacketPlayersUpdate(tick_t tick, const QHash<plid_t, QString> &players);

	tick_t getTick() const;
	void setTick(tick_t tick);

	QHash<plid_t, QString> getPlayers() const;
	void setPlayers(const QHash<plid_t, QString> &players);

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	tick_t tick;
	QHash<plid_t, QString> players;
};

// The following class relies on this assumption.
static_assert(std::is_same<plid_t, quint8>::value, "plid_t != quint8");

/*
 * This class stores the leaderboard as a 1d array alternating
 * between player id and score.
 */
class PacketLeaderboardUpdate : public Packet
{
public:
	PacketLeaderboardUpdate();
	PacketLeaderboardUpdate(tick_t tick, const quint8 lb[10]);

	tick_t getTick() const;
	void setTick(tick_t tick);

	const quint8 *getLeaderboard() const;
	void setLeaderboard(const quint8 lb[10]);

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	tick_t tick;
	quint8 data[10];
};

/*
 * This class either holds pointers to the board
 * state or its own copy.
 */
class PacketResendBoard : public Packet
{
public:
	/*
	 * Initializes a new PacketResendBoard with its own copy
	 * of the board state.
	 */
	PacketResendBoard();
	/*
	 * Initializes a new PacketResendBoard holding pointers to
	 * the board state.
	 */
	PacketResendBoard(tick_t tick, state_t *board[CLIENT_FRAME]);
	/*
	 * Copy constructor.
	 */
	PacketResendBoard(const PacketResendBoard &other);
	~PacketResendBoard();

	PacketResendBoard &operator =(const PacketResendBoard &other);

	tick_t getTick() const;
	void setTick(tick_t tick);

	const state_t *const *getBoard() const;
	/*
	 * Sets the board state to be the given pointer.
	 */
	void setBoardPointer(state_t *board[CLIENT_FRAME]);
	/*
	 * Copies the board state from the given pointer.
	 */
	void setBoardCopy(state_t const *board[CLIENT_FRAME]);

	QByteArray getChecksum() const;

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	tick_t tick;

	bool alloc;
	state_t *board[CLIENT_FRAME];

	QByteArray chksum;

	void allocBoard();
};

class PacketGameJoin : public Packet
{
public:
	/*
	 * N.B. This initializes the sub-packets with defauly constructors
	 * which means PacketResendBoard will allocate its own CLIENT_FRAME^2
	 * array of state_t's. As a result, this constructor should only
	 * be used if this packet is going to be read in and the other constructor
	 * should be used for writing.
	 */
	PacketGameJoin();
	PacketGameJoin(plid_t plid, quint8 score, const PacketPlayersUpdate &ppu, const PacketLeaderboardUpdate &plu, const PacketResendBoard &prb);

	plid_t getId() const;
	void setId(plid_t id);

	quint8 getScore() const;
	void setScore(quint8 score);

	const PacketPlayersUpdate &getPPU() const;
	void setPPU(const PacketPlayersUpdate &ppu);

	const PacketLeaderboardUpdate &getPLU() const;
	void setPLU(const PacketLeaderboardUpdate &plu);

	const PacketResendBoard &getPRB() const;
	void setPRB(const PacketResendBoard &prb);

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	plid_t plid;
	quint8 score;

	PacketPlayersUpdate ppu;
	PacketLeaderboardUpdate plu;
	PacketResendBoard prb;
};

/*
 * This class either holds pointers to the diff or its own copy.
 */
class PacketGameTick : public Packet
{
public:
	/*
	 * Initializes a new PacketGameTick with its own copy of the diff.
	 */
	PacketGameTick();
	/*
	 * Initializes a new PacketGameTick holding pointers to the diff.
	 */
	PacketGameTick(tick_t tick, Direction dir, quint8 score, const state_t news[CLIENT_FRAME], state_t *diff[CLIENT_FRAME], const QByteArray &chksum);
	/*
	 * Copy constructor.
	 */
	PacketGameTick(const PacketGameTick &other);
	~PacketGameTick();

	PacketGameTick &operator =(const PacketGameTick &other);

	tick_t getTick() const;
	void setTick(tick_t tick);

	Direction getDirection() const;
	void setDirection(Direction dir);

	quint8 getScore() const;
	void setScore(quint8 sc);

	const state_t *getNewSection() const;
	void setNewSection(const state_t[CLIENT_FRAME]);

	const state_t *const *getDiff() const;
	/*
	 * Sets the diff to be the given pointer.
	 */
	void setDiffPointer(state_t *diff[CLIENT_FRAME], const QByteArray &checksum);
	/*
	 * Copies the diff from the given pointer.
	 */
	void setDiffCopy(state_t const *diff[CLIENT_FRAME], const QByteArray &checksum);

	QByteArray getChecksum() const;

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	tick_t tick;
	quint8 dir;
	quint8 score;

	state_t news[CLIENT_FRAME];

	bool alloc;
	state_t *diff[CLIENT_FRAME];

	QByteArray chksum;

	void allocDiff();
};

class PacketUpdateDir : public Packet
{
public:
	PacketUpdateDir();
	PacketUpdateDir(Direction dir);

	Direction getDirection() const;
	void setDirection(Direction dir);

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	quint8 dir;
};

class PacketRequestResend : public Packet
{
public:
	PacketRequestResend()
		: Packet(PACKET_REQUEST_RESEND)
	{
	}
};

class PacketGameEnd : public Packet
{
public:
	PacketGameEnd();
	PacketGameEnd(quint8 score);

	quint8 getScore() const;
	void setScore(quint8 score);

protected:
	void read(QDataStream &str) override;
	void write(QDataStream &str) const override;

private:
	quint8 score;
};

#endif // !PROTOCOL_H
