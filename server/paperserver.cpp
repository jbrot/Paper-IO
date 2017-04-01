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
	QThread *cthrd = new QThread(this);
	ClientHandler *chand = new ClientHandler;
	chand->moveToThread(cthrd);
	connect(chand, &ClientHandler::disconnected, cthrd, &QThread::quit);
	connect(cthrd, &QThread::finished, chand, &QObject::deleteLater);
	cthrd->start();

	QMetaObject::invokeMethod( chand, "establishConnection", Q_ARG(int, socketDescriptor));

	ThreadClient tc{cthrd, chand};
	connections.append(tc);
}
