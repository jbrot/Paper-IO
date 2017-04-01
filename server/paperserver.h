/*
 * This class is the actual server. It listens for connections
 * and assigns them games.
 */

#ifndef PAPERSERVER_H
#define PAPERSERVER_H

#include <QHash>
#include <QQueue>
#include <QTcpServer>
#include <QThread>

#include "clienthandler.h"
#include "gamehandler.h"

struct ThreadClient
{
	bool established;
	QThread *thread;
	ClientHandler *client;
};

struct ThreadGame
{
	QThread *thread;
	GameHandler *game;
};

class PaperServer : public QTcpServer
{
	Q_OBJECT

public:
	PaperServer(QObject *parent = 0);
	~PaperServer();

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	void ioError(thid_t source, QAbstractSocket::SocketError err, QString msg);
	void validateConnection(thid_t id);
	void queueConnection(thid_t id);
	void deleteConnection(thid_t id);
	void launchGame();
	void gameTerminated(gid_t id);

private:
	QHash<gid_t, ThreadGame> games;

	// TODO Add locks
	QHash<thid_t, ThreadClient> connections;
	QQueue<thid_t> waiting;
};

#endif // !PAPERSERVER_H
