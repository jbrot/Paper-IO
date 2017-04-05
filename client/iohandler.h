/*
 * This class manages the actual connection with the server. It is
 * meant to be run on its own thread and interfaced with via queued
 * signal/slot connections.
 */

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QDataStream>
#include <QDateTime>
#include <QTcpSocket>
#include <QTimer>

#include "clientgamestate.h"
#include "types.h"

class IOHandler : public QObject
{
	Q_OBJECT

public:
	IOHandler(ClientGameState &cgs, QObject *parent = Q_NULLPTR);

public slots:
	void connectToServer(const QString &host, quint16 port, const QString &name);
	void abort();
	void disconnect();

	void enterQueue();
	void changeDirection(Direction dir);
	void requestResend();

signals:
	void connected();
	void disconnected();
	void error(QAbstractSocket::SocketError error, QString msg);

	void queued();
	void enteredGame();
	void gameEnded(quint8 score);

private slots:
	void ierror(QAbstractSocket::SocketError error);
	void kaTimeout();
	void newData();

private:
	QTcpSocket *socket;
	QDataStream str;

	QTimer *keepAlive;
	QDateTime lastka;

	QString name;
	ClientGameState &cgs;

	/*
	 * WARNING: This function must be called within a lock.
	 */
	void updatePlayerPositions();

	void processPlayersUpdate(const PacketPlayersUpdate &ppu, bool nested = false);
	void processLeaderboardUpdate(const PacketLeaderboardUpdate &plu, bool nested = false);
	void processFullBoard(const PacketResendBoard &prb, bool nested = false);
	void processJoinGame(const PacketGameJoin &pgj);
	void processGameTick(const PacketGameTick &pgt);
};

#endif // !IOHANDLER_H
