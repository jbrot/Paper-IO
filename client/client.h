/*
 * This class is the main window of the Paper-IO client and
 * handles most of the high-level set up.
 */

#ifndef Client_H
#define Client_H

#include <QNetworkSession>
#include <QTimer>
#include <QThread>
#include <QWidget>

#include "iohandler.h"
#include "launcher.h"

class Client : public QWidget
{
	Q_OBJECT

public:
	Client(QWidget *parent = 0);
	~Client();

	QSize sizeHint() const override;

private slots:
	void displayError(QAbstractSocket::SocketError socketError, QString msg);
	void displayError2(QNetworkSession::SessionError sessionError);
	void sessionOpened();
	void connected();
	void disconnected();
	void connectTimeout();

private:
	Launcher *launcher;

	QTimer *timeout;
	IOHandler *ioh;
	QThread *iothread;

	QNetworkSession *session;
};

#endif // !CLIENT_H
