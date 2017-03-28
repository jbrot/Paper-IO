/*
 * This class is the actual server. It listens for connections
 * and assigns them games.
 */

#ifndef PAPERSERVER_H
#define PAPERSERVER_H

#include <QTcpServer>
#include <QThread>
#include <QVector>

#include "clienthandler.h"

struct ThreadClient
{
	QThread *thread;
	ClientHandler *client;
};

class PaperServer : public QTcpServer
{
	Q_OBJECT

public:
	PaperServer(QObject *parent = 0);
	~PaperServer();

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private:
	QVector<ThreadClient> connections;
};

#endif // !PAPERSERVER_H
