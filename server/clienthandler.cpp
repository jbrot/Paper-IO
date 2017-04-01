/*
 * Implements IO for a client.
 */

#include <QHostAddress>

#include "clienthandler.h"
#include "protocol.h"

quint32 ClientHandler::idCount = 0;

ClientHandler::ClientHandler(QObject *parent)
	: id(idCount)
	, keepAlive(new QTimer(this))
	, socket(new QTcpSocket(this))
	, state(LIMBO)
	, player(NULL_ID)
{
	// Note that due to not locking this makes the constructor not
	// thread safe.
	ClientHandler::idCount++;

	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	keepAlive->setInterval(5000);
	connect(keepAlive, &QTimer::timeout, this, &ClientHandler::kaTimeout);
	lastka = QDateTime::currentDateTime();

	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
	        this, &ClientHandler::ierror);
	connect(socket, &QAbstractSocket::disconnected, this, &ClientHandler::disconnected);
	connect(socket, &QAbstractSocket::disconnected, keepAlive, &QTimer::stop);
	connect(socket, &QIODevice::readyRead, this, &ClientHandler::newData);
}

void ClientHandler::transitionState(ClientState news, plid_t pd)
{
	state = news;
	player= pd;

	// TODO Inform client of transition
}

void ClientHandler::establishConnection(int socketDescriptor)
{
	if (!socket->setSocketDescriptor(socketDescriptor))
	{
		ierror(socket->error());
		return;
	}

	keepAlive->start();

	qDebug() << "Connection " << id << " established with: " << socket->peerAddress(); 

	emit connected();
}

void ClientHandler::sendTick(void *igs, tick_t tick)
{
	// TODO Send update
}

void ClientHandler::abort()
{
	socket->abort();
	keepAlive->stop();
}

void ClientHandler::disconnect()
{
	socket->disconnectFromHost();
}

void ClientHandler::ierror(QAbstractSocket::SocketError err)
{
	emit error(err, socket->errorString());
}

void ClientHandler::kaTimeout()
{
	if (lastka.secsTo(QDateTime::currentDateTime()) > TIMEOUT_LEN)
	{
		qDebug() << "Connection " << id << ": Haven't received keep alive packet, timing out client.";
		disconnect();
	}
	str << PACKET_KEEP_ALIVE;
	qDebug() << "Connection " << id << ": Keep alive sent!";
}

void ClientHandler::newData()
{
	packet_t pkth = 0;

	str.startTransaction();
	str >> pkth;
	// We have to do two switches---first to read in the packet and then,
	// once reading is confirmed successful, to process.
	switch (pkth) {
	case PACKET_KEEP_ALIVE:
		break;
	default:
		break;
	}
	// If we didn't fully read the packet, then quit.
	if (!str.commitTransaction())
		return;

	switch (pkth) {
	case PACKET_KEEP_ALIVE:
		lastka = QDateTime::currentDateTime();
		qDebug() << "Connection " << id << ": Keep alive received!";
		break;
	default:
		qDebug() << "Connection " << id << ": Received unknown packet: " << pkth;
		break;
	}
}

thid_t ClientHandler::getId()
{
	return id;
}
