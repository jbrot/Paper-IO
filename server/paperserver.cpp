/*
 * Implements PaperIOSever, which listens for new connections and assigns them
 * to a game.
 */

#include "paperserver.h"

PaperServer::PaperServer(QObject *parent) 
	: QTcpServer(parent)
	, games()
	, connections()
	, waiting()
{
}

PaperServer::~PaperServer()
{
	// TODO Kill the game threads.
	// Kill all of the IO threads.
	for (auto iter = connections.cbegin(); iter != connections.cend(); iter++)
	{
		ThreadClient tc = iter.value();
		tc.thread->quit();
		if (!tc.thread->wait(1000)) // Wait for termination (1 sec max)
		{
			qDebug() << "Connection " << iter.key() << " has not quit after 1 second. Terminating...";
			tc.thread->terminate();
			tc.thread->wait();
		}
	}
}

void PaperServer::incomingConnection(qintptr socketDescriptor)
{
	QThread *cthrd = new QThread(this);
	ClientHandler *chand = new ClientHandler;
	thid_t id = chand->getId();
	chand->moveToThread(cthrd);

	connect(chand, &ClientHandler::error, [id,this] (QAbstractSocket::SocketError error, QString msg) {
		this->ioError(id, error, msg);
	} ); 
	connect(chand, &ClientHandler::connected, [id,this] {
		this->validateConnection(id);
	} ); 
	connect(chand, &ClientHandler::disconnected, cthrd, &QThread::quit);
	connect(chand, &ClientHandler::requestJoinGame, [id,this] {
		this->queueConnection(id);
	} ); 

	connect(cthrd, &QThread::finished, chand, &QObject::deleteLater);
	connect(cthrd, &QThread::finished, this, [id,this] {
		this->deleteConnection(id);
	} );

	cthrd->start();

	ThreadClient tc{false, cthrd, chand};
	connections.insert(chand->getId(), tc);

	QMetaObject::invokeMethod( chand, "establishConnection", Q_ARG(int, socketDescriptor));
}

void PaperServer::ioError(thid_t id, QAbstractSocket::SocketError err, QString msg)
{
	qWarning() << "Connection " << id << ": Socket Error " << err << ": " << msg;

	if (!connections.contains(id))
	{
		qWarning() << "Warning: Connection " << id << " is not registered but claims to have a Socket Error!";
		return;
	}

	// If the connection is established, it will most likely emit a disconnected event and be removed there
	// However, if it is not established, then the socket couldn't be opened so we need to remove it from the map.
	ThreadClient tc = connections.value(id);
	if (tc.established)
		return;
	connections.remove(id);
}

void PaperServer::validateConnection(thid_t id)
{
	if (!connections.contains(id))
	{
		qWarning() << "Warning: Connection " << id << " is not registered but claims to be established!";
		return;
	}

	ThreadClient tc = connections.value(id);
	tc.established = true;
	connections.insert(id, tc);
}

void PaperServer::queueConnection(thid_t id)
{
	if (!connections.contains(id))
	{
		qWarning() << "Warning: Connection " << id << " is not registered but requests to be queued!";
		return;
	}

	waiting.enqueue(id);
	ThreadClient tc = connections.value(id);
	QMetaObject::invokeMethod( tc.client, "enqueue");

	// TODO Create game if none exist. If the queue size is too large, set a timer
	// to start a new game if the queue doesn't shrink in a little while.
}

void PaperServer::deleteConnection(thid_t id)
{
	waiting.removeAll(id);
	if (connections.remove(id))
	{
		qDebug() << "Connection " << id << " closed.";
	} else {
		qWarning() << "Warning: Connection " << id << " is not registered but claims to be terminated!";
	}
}

void PaperServer::launchGame()
{
	// TODO
}

void PaperServer::gameTerminated(gid_t id)
{
	// TODO
}
