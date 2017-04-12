/*
 * The ClientHandler class manages IO with a single client.
 */

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QDataStream>
#include <QDateTime>
#include <QReadWriteLock>
#include <QTcpSocket>
#include <QTimer>

#include "gamestate.h"
#include "protocol.h"
#include "types.h"

/*
 * The current state of the client. QUEUEING means the client
 * wants to join a game but has not been assigned one yet.
 * INGAME means the client is currently assigned to a game.
 * LIMBO means the client has lost a game and has not yet
 * decided it wants to play another game.
 */
enum ClientState {
	QUEUEING,
	INGAME,
	LIMBO,
};

/* Connection ID */
typedef quint32 thid_t;

class ClientHandler : public QObject
{
	Q_OBJECT

public:
	/*
	 * WARNING: This constructor is NOT thread safe. Make
	 * sure you only construct one ClientHandler at a time.
	 */
	ClientHandler(QObject *parent = Q_NULLPTR);

	thid_t getId() const;

public slots:
	void enqueue();
	void beginGame(plid_t id, GameState *gs);
	void endGame(score_t score);
	void establishConnection(int socketDescriptor);
	void sendTick();
	void abort();
	void disconnect();

signals:
	void error(QAbstractSocket::SocketError error, QString msg);
	void connected();
	void disconnected();
	void requestJoinGame(const QString &name);
	void changeDirection(Direction dir);
	void requestResync();

private slots:
	void ierror(QAbstractSocket::SocketError error);
	void kaTimeout();
	void newData();

private:
	static thid_t idCount;

	const thid_t id;
	QTimer *keepAlive;
	QTcpSocket *socket;
	QDataStream str;

	ClientState state;
	GameState *gs;
	plid_t player;
	QString name;

	QDateTime lastka;

	PacketPlayersUpdate makePPU();
	PacketLeaderboardUpdate makePLU();
	PacketResendBoard makePRB();
};

#endif // !CLIENTHANDLER_H
