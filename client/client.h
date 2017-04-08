/*
 * This class is the main window of the Paper-IO client and
 * handles most of the high-level set up.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <QNetworkSession>
#include <QTimer>
#include <QThread>
#include <QWidget>

#include "clientgamestate.h"
//#include "gamerenderer.h"
#include "gameover.h"
#include "iohandler.h"
#include "launcher.h"
#include "waiting.h"

class Client : public QWidget
{
	Q_OBJECT

public:
	Client(QWidget *parent = Q_NULLPTR);
	~Client();

	QSize sizeHint() const override;

private slots:
	void displayError(QAbstractSocket::SocketError socketError, QString msg);
	void displayError2(QNetworkSession::SessionError sessionError);
	void sessionOpened();
	void disconnected();
	void connectTimeout();

private:
	Launcher *launcher;

	ClientGameState cgs;

	QTimer *timeout;
	IOHandler *ioh;
	QThread *iothread;

	Waiting *waiting;

//	QTimer *rtimer;
//	GameRenderer *render;

	GameOver *gameover;

	QNetworkSession *session;
};

#endif // !CLIENT_H
