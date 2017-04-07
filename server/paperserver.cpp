/*
 * Implements PaperIOSever, which listens for new connections and assigns them
 * to a game.
 */

#include "paperserver.h"

struct PaperServer::ThreadClient
{
	bool established;
	QThread *thread;
	ClientHandler *client;
	QString name;
};

struct PaperServer::ThreadGame
{
	QThread *thread;
	GameHandler *game;
};

// If more than this many players are queueing, then we start a new game.
const int MAX_QUEUE = 2;

PaperServer::PaperServer(QObject *parent) 
	: QTcpServer(parent)
	, games()
	, ctclock()
	, connections()
	, waiting()
	, ngt(new QTimer(this))
{
	ngt->setInterval(5000);
	connect(ngt, &QTimer::timeout, this, &PaperServer::launchGame);
}

PaperServer::~PaperServer()
{
	// Kill the game threads.
	for (auto iter = games.cbegin(); iter != games.cend(); iter++)
	{
		ThreadGame tg = iter.value();
		tg.thread->quit();
		if (!tg.thread->wait(1000)) // Wait for termination (1 sec max)
		{
			qDebug() << "Game" << iter.key() << "has not quit after 1 second. Terminating...";
			tg.thread->terminate();
			tg.thread->wait();
		}
	}

	// Kill all of the IO threads.
	for (auto iter = connections.cbegin(); iter != connections.cend(); iter++)
	{
		ThreadClient tc = iter.value();
		tc.thread->quit();
		if (!tc.thread->wait(1000)) // Wait for termination (1 sec max)
		{
			qDebug() << "Connection" << iter.key() << "has not quit after 1 second. Terminating...";
			tc.thread->terminate();
			tc.thread->wait();
		}
	}
}

void PaperServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "Received connection!";
	QThread *cthrd = new QThread(this);
	ClientHandler *chand = new ClientHandler;
	thid_t id = chand->getId();
	chand->moveToThread(cthrd);

	connect(chand, &ClientHandler::error, [id,this] (QAbstractSocket::SocketError error, QString msg) {
		this->ioError(id, error, msg);
	} ); 
	connect(chand, &ClientHandler::connected, this, [id,this] {
		this->validateConnection(id);
	} ); 
	connect(chand, &ClientHandler::disconnected, cthrd, &QThread::quit);
	connect(chand, &ClientHandler::requestJoinGame, this, [id,this] (const QString &name) {
		this->queueConnection(id, name);
	} ); 

	connect(cthrd, &QThread::finished, chand, &QObject::deleteLater);
	connect(cthrd, &QThread::finished, this, [id,this] {
		this->deleteConnection(id);
	} );

	cthrd->start();

	ThreadClient tc{false, cthrd, chand, QLatin1String("")};
	ctclock.lock();
	connections.insert(id, tc);
	ctclock.unlock();

	QMetaObject::invokeMethod(chand, "establishConnection", Q_ARG(int, socketDescriptor));
}

void PaperServer::ioError(thid_t id, QAbstractSocket::SocketError err, QString msg)
{
	qWarning() << "Connection " << id << ": Socket Error " << err << ": " << msg;

	ctclock.lock();
	if (!connections.contains(id))
	{
		ctclock.unlock();
		qWarning() << "Warning: Connection " << id << " is not registered but claims to have a Socket Error!";
		return;
	}

	// If the connection is established, it will most likely emit a disconnected event and be removed there
	// However, if it is not established, then the socket couldn't be opened so we need to remove it from the map.
	ThreadClient tc = connections.value(id);
	if (!tc.established)
		connections.remove(id);
	ctclock.unlock();
}

void PaperServer::validateConnection(thid_t id)
{
	ctclock.lock();
	if (!connections.contains(id))
	{
		ctclock.unlock();
		qWarning() << "Warning: Connection" << id << "is not registered but claims to be established!";
		return;
	}

	ThreadClient tc = connections.value(id);
	tc.established = true;
	connections.insert(id, tc);
	ctclock.unlock();

	qDebug() << "Connection" << id << "validated!";
}

void PaperServer::queueConnection(thid_t id, const QString &name)
{
	ctclock.lock();
	if (!connections.contains(id))
	{
		ctclock.unlock();
		qWarning() << "Warning: Connection" << id << "is not registered but requests to be queued!";
		return;
	}

	waiting.enqueue(id);
	ThreadClient tc = connections.value(id);
	if (!name.isEmpty())
		tc.name = name;
	QMetaObject::invokeMethod( tc.client, "enqueue");

	ctclock.unlock();

	qDebug() << "Connection" << id << "queued.";

	if (!games.size())
		launchGame();
	else if (waiting.size() > MAX_QUEUE && !ngt->isActive())
		ngt->start();
}

void PaperServer::deleteConnection(thid_t id)
{
	ctclock.lock();
	waiting.removeAll(id);
	if (connections.remove(id))
	{
		qDebug() << "Connection " << id << " closed.";
	} else {
		qWarning() << "Warning: Connection " << id << " is not registered but claims to be terminated!";
	}
	ctclock.unlock();
}

void PaperServer::launchGame()
{
	// We only launch a game if our queue is large enough (and we have games running)
	if (games.size() && waiting.size() <= MAX_QUEUE)
	{
		ngt->stop();
		return;
	}

	QThread *gthrd = new QThread(this);
	// Note we can customize game properties here.
	GameHandler *ghand = new GameHandler(*this);
	gid_t id = ghand->getId();
	ghand->moveToThread(gthrd);

	connect(ghand, &GameHandler::terminated, gthrd, &QThread::quit);

	connect(gthrd, &QThread::finished, ghand, &QObject::deleteLater);
	connect(gthrd, &QThread::finished, this, [id,this] {
		this->deleteGame(id);
	} );

	gthrd->start();

	ThreadGame gc{gthrd, ghand};
	games.insert(id, gc);

	QMetaObject::invokeMethod(ghand, "startGame");

	qDebug() << "Game" << id << "launched.";
}

void PaperServer::deleteGame(gid_t id)
{
	if (!games.remove(id))
		qDebug() << "Game" << id << " reports being terminated, but is not registered!";
}

QList<QPair<ClientHandler *, QString>> PaperServer::dequeueClients(int num)
{
	QList<QPair<ClientHandler *, QString>> ret;

	ctclock.lock();
	ret.reserve(std::min(waiting.size(), num));
	
	for (auto iter = waiting.begin(); iter < waiting.end() && ret.size() < num; iter = waiting.erase(iter))
	{
		thid_t id = *iter;
		if (!connections.contains(id))
		{
			qWarning() << "Connection" << id << "is queued but is not registered!";
			continue;
		}
	
		ThreadClient tc = connections.value(id);
		ret.append(qMakePair(tc.client, tc.name));
	}

	ctclock.unlock();

	return ret;
}
