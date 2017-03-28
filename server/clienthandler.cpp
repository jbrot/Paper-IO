/*
 * Implements IO for a client.
 */

#include <QHostAddress>

#include "clienthandler.h"
#include "protocol.h"

ClientHandler::ClientHandler(QObject *parent)
	: keepAlive(new QTimer(this))
	, socket(new QTcpSocket(this))
	, state(LIMBO)
	, player(NULL_ID)
{
	// We need to do this so we can communicate errors across threads.
	qRegisterMetaType<QAbstractSocket::SocketError>();

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

	qDebug() << "New connection: " << socket->peerAddress(); 
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
		disconnect();
		qDebug() << "Haven't received keep alive packet, timing out client.";
	}
	str << PACKET_KEEP_ALIVE;
	qDebug() << "Keep alive sent!";
}

void ClientHandler::newData()
{
	qint8 packet = 0;

	str.startTransaction();
	str >> packet;
	// We have to do two switches---first to read in the packet and then,
	// once reading is confirmed successful, to process.
	switch (packet) {
	case PACKET_KEEP_ALIVE:
		break;
	default:
		break;
	}
	// If we didn't fully read the packet, then quit.
	if (!str.commitTransaction())
		return;

	switch (packet) {
	case PACKET_KEEP_ALIVE:
		lastka = QDateTime::currentDateTime();
		qDebug() << "Keep alive received!";
		break;
	default:
		qDebug() << "Received unknown packet: " << packet;
		break;
	}
}
