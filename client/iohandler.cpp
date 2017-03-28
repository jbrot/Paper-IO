/*
 * This is the implementation of the IOHandler class. This
 * is where the nitty gritty network details live.
 */

#include "iohandler.h"
#include "protocol.h"

// We initialize the socket here with this object as its parent.
// That way, when moved to our own thread, the socket will be moved
// with us. The QDataStream is not a QObject, so as long as we're
// careful about which thread we use it from, we should be fine.
IOHandler::IOHandler(QObject *parent)
	: QObject(parent)
	, socket(new QTcpSocket(this))
	, keepAlive(new QTimer(this))
{
	// We need to do this so we can communicate errors across threads.
	qRegisterMetaType<QAbstractSocket::SocketError>();

	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	keepAlive->setInterval(5000);
	connect(keepAlive, &QTimer::timeout, this, &IOHandler::kaTimeout);

	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
	        this, &IOHandler::ierror);
	connect(socket, &QAbstractSocket::connected, this, &IOHandler::connected);
	connect(socket, &QAbstractSocket::connected, keepAlive, static_cast<void (QTimer::*)()>(&QTimer::start));
	connect(socket, &QAbstractSocket::connected, this, [this] {
		this->lastka = QDateTime::currentDateTime();
	});
	connect(socket, &QAbstractSocket::disconnected, this, &IOHandler::disconnected);
	connect(socket, &QAbstractSocket::disconnected, keepAlive, &QTimer::stop);
	connect(socket, &QIODevice::readyRead, this, &IOHandler::newData);
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
	keepAlive->stop();
}

void IOHandler::disconnect()
{
	socket->disconnectFromHost();
}

void IOHandler::ierror(QAbstractSocket::SocketError err)
{
	emit error(err, socket->errorString());
}

void IOHandler::kaTimeout()
{
	if (lastka.secsTo(QDateTime::currentDateTime()) > TIMEOUT_LEN)
	{
		disconnect();
		qDebug() << "Haven't received keep alive packet, timing out client.";
		return;
	}
	str << PACKET_KEEP_ALIVE;
	qDebug() << "Sent keep alive!";
}

void IOHandler::newData()
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
