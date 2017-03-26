/*
 * This class is the actual server. It listens for connections
 * and assigns them games.
 */

#ifndef PAPERSERVER_H
#define PAPERSERVER_H

#include <QTcpServer>

class PaperServer : public QTcpServer
{
	Q_OBJECT

public:
	PaperServer(QObject *parent = 0);

protected:
	void incomingConnection(qintptr socketDescriptor) override;
};

#endif // !PAPERSERVER_H
