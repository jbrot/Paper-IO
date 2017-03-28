/*
 * This class manages the actual connection with the server. It is
 * meant to be run on its own thread and interfaced with via queued
 * signal/slot connections.
 */

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QDataStream>
#include <QDateTime>
#include <QTcpSocket>
#include <QTimer>

class IOHandler : public QObject
{
	Q_OBJECT

public:
	IOHandler(QObject *parent = Q_NULLPTR);

public slots:
	void connectToServer(const QString &host, quint16 port);
	void abort();
	void disconnect();

signals:
	void connected();
	void disconnected();
	void error(QAbstractSocket::SocketError error, QString msg);

private slots:
	void ierror(QAbstractSocket::SocketError error);
	void kaTimeout();
	void newData();

private:
	QTcpSocket *socket;
	QDataStream str;

	QTimer *keepAlive;
	QDateTime lastka;
};

#endif // !IOHANDLER_H
