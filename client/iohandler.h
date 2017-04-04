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

#include "types.h"

class IOHandler : public QObject
{
	Q_OBJECT

public:
	IOHandler(QObject *parent = Q_NULLPTR);

public slots:
	void connectToServer(const QString &host, quint16 port, const QString &name);
	void abort();
	void disconnect();

	void enterQueue();
	void changeDirection(Direction dir);
	void requestResend();

signals:
	void connected();
	void disconnected();
	void error(QAbstractSocket::SocketError error, QString msg);

	void queued();
	void enteredGame();
	void gameEnded(quint8 score);

private slots:
	void ierror(QAbstractSocket::SocketError error);
	void kaTimeout();
	void newData();

private:
	QTcpSocket *socket;
	QDataStream str;

	QTimer *keepAlive;
	QDateTime lastka;

	QString name;
};

#endif // !IOHANDLER_H
