/*
 * The ClientThread class manages IO with a single client.
 */

#include <QDataStream>
#include <QDateTime>
#include <QTcpSocket>
#include <QTimer>

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

class ClientHandler : public QObject
{
	Q_OBJECT

public:
	ClientHandler(QObject *parent = Q_NULLPTR);

public slots:
	void transitionState(ClientState news, plid_t pid = NULL_ID);
	void establishConnection(int socketDescriptor);
	void sendTick(void *igs, tick_t tick);
	void abort();
	void disconnect();

signals:
	void error(QAbstractSocket::SocketError error, QString msg);
	void disconnected();
	void requestJoinGame();
	void changeDirection(Direction dir);
	void requestResync();

private slots:
	void ierror(QAbstractSocket::SocketError error);
	void kaTimeout();
	void newData();

private:
	QTimer *keepAlive;
	QTcpSocket *socket;
	QDataStream str;

	ClientState state;
	plid_t player;

	QDateTime lastka;
};
