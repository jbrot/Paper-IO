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
	bool established;
	QThread *thread;
	ClientHandler *client;
};

class PaperServer : public QTcpServer
{
	Q_OBJECT

public:
	PaperServer(QObject *parent = 0);
	~PaperServer();

public slots:
	void ioError(thid_t source, QAbstractSocket::SocketError err, QString msg);
	void validateConnection(thid_t id);
	void queueConnection(thid_t id);
	void deleteConnection(thid_t id);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private:
	QHash<thid_t, ThreadClient> connections;
};

#endif // !PAPERSERVER_H
