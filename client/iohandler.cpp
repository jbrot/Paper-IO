/*
 * This is the implementation of the IOHandler class. This
 * is where the nitty gritty network details live.
 */

#include "iohandler.h"

// We initialize the socket here with this object as its parent.
// That way, when moved to our own thread, the socket will be moved
// with us. The QDataStream is not a QObject, so as long as we're
// careful about which thread we use it from, we should be fine.
IOHandler::IOHandler(QObject *parent)
	: QObject(parent)
	, socket(new QTcpSocket(this))
{
	// We need to do this so we can communicate errors across threads.
	qRegisterMetaType<QAbstractSocket::SocketError>();

	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
	        this, &IOHandler::ierror);
	connect(socket, &QAbstractSocket::connected, this, &IOHandler::connected);
	connect(socket, &QAbstractSocket::disconnected, this, &IOHandler::disconnected);
}

void IOHandler::connectToServer(const QString &host, quint16 port)
{
	// The abort call is kind of overkill, but it's better to be safe than sorry
	socket->abort();
	socket->connectToHost(host, port);
}

void IOHandler::abort()
{
	socket->abort();
}

void IOHandler::disconnect()
{
	socket->disconnectFromHost();
}

void IOHandler::ierror(QAbstractSocket::SocketError err)
{
	emit error(err, socket->errorString());
}
