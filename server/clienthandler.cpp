/*
 * Implements IO for a client.
 */

#include "clienthandler.h"

ClientHandler::ClientHandler(QObject *parent)
	: keepAlive(new QTimer(this))
	, socket(new QTcpSocket(this))
	, state(QUEUEING)
	, player(NULL_ID)
{
	// We need to do this so we can communicate errors across threads.
	qRegisterMetaType<QAbstractSocket::SocketError>();

	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	keepAlive->setInterval(5000);
	connect(keepAlive, &QTimer::timeout, this, &ClientHandler::kaTimeout);

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

void ClientHandler::establishConnection(qintptr socketDescriptor)
{
	if (!socket->setSocketDescriptor(socketDescriptor))
	{
		ierror(socket->error());
		return;
	}

	keepAlive->start();
	// Communicate the state to the client.
	transitionState(state, player);
}

void sendTick(InternalGameState *igs, tick_t tick)
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
	// TODO Send keepalive
}

void ClientHandler::newData()
{
	// TODO Read packet
}
