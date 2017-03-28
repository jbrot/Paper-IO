/*
 * Implements PaperIOSever, which listens for new connections and assigns them
 * to a game.
 */

#include "paperserver.h"

PaperServer::PaperServer(QObject *parent) 
	: QTcpServer(parent)
{
	// Nothing to do yet
}

PaperServer::~PaperServer()
{
	// Kill all of the IO threads.
	for (auto iter = connections.cbegin(); iter < connections.cend(); iter++)
	{
		ThreadClient tc = *iter;
		tc.thread->quit();
		if (!tc.thread->wait(1000)) // Wait for termination (1 sec max)
		{
			tc.thread->terminate();
			tc.thread->wait();
		}
	}
}

void PaperServer::incomingConnection(qintptr socketDescriptor)
{
	QThread *cth = new QThread(this);
	ClientHandler *ch = new ClientHandler;
	cth->moveToThread(cth);
	connect(ch, &ClientHandler::disconnect, cth, &QThread::quit);
	connect(cth, &QThread::finished, ch, &QObject::deleteLater);
	cth->start();

	QMetaObject::invokeMethod( ch, "establishConnection", Q_ARG(qintptr, socketDescriptor));

	ThreadClient tc{cth, ch};
	connections.append(tc);
}
