/*
 * This class is the actual server. It listens for connections
 * and assigns them games.
 */

#ifndef PAPERSERVER_H
#define PAPERSERVER_H

#include <QHash>
#include <QPair>
#include <QQueue>
#include <QTcpServer>
#include <QThread>

#include "gamehandler.h"
#include "clienthandler.h"

class PaperServer : public QTcpServer
{
	Q_OBJECT

public:
	PaperServer(QObject *parent = 0);
	~PaperServer();

	QList<QPair<ClientHandler *, QString>> dequeueClients(int num);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	void ioError(thid_t source, QAbstractSocket::SocketError err, QString msg);
	void validateConnection(thid_t id);
	void queueConnection(thid_t id, const QString &name);
	void deleteConnection(thid_t id);
	void launchGame();
	void deleteGame(gid_t id);

private:
	struct ThreadClient;
	struct ThreadGame;
	QHash<gid_t, ThreadGame> games;

	QMutex ctclock;
	QHash<thid_t, ThreadClient> connections;
	QQueue<thid_t> waiting;

	QTimer *ngt;
};

#endif // !PAPERSERVER_H
