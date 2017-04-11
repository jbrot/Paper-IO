/*
 * This class is the main window of the Paper-IO client and
 * handles most of the high-level set up.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <QNetworkSession>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QThread>
#include <QWidget>

#include "arduino.h"
#include "clientgamestate.h"
#include "gamewidget.h"
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
	void displayError3(QSerialPort::SerialPortError error, QString msg);
	void sessionOpened();
	void disconnected();
	void connectTimeout();
	void connectToArduino();

private:
	Launcher *launcher;

	ClientGameState cgs;

	QTimer *timeout;
	IOHandler *ioh;
	QThread *iothread;

	Arduino *arduino;

	Waiting *waiting;

	QTimer *rtimer;
	GameWidget *render;

	GameOver *gameover;

	QNetworkSession *session;
};

#endif // !CLIENT_H
