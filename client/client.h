/*
 * This class is the main window of the Paper-IO client and
 * handles most of the high-level set up.
 */

#ifndef Client_H
#define Client_H

#include <QDataStream>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkSession>
#include <QPushButton>
#include <QTimer>
#include <QThread>
#include <QWidget>

#include "iohandler.h"

class Client : public QWidget
{
	Q_OBJECT

public:
	Client(QWidget *parent = 0);
	~Client();

	QSize sizeHint() const override;

private slots:
	void openConnection();
	void displayError(QAbstractSocket::SocketError socketError, QString msg);
	void displayError2(QNetworkSession::SessionError sessionError);
	void sessionOpened();
	void enableConnect();
	void connected();
	void disconnected();
	void connectTimeout();

signals:
	void connectToServer(const QString &host, quint16 port);

private:
	QLabel *status;
	QLineEdit *nameEdit;
	QLineEdit *ipEdit;
	QLineEdit *portEdit;
	QPushButton *ctc;

	QTimer *timeout;
	IOHandler *ioh;
	QThread *iothread;

	QNetworkSession *session;
};

#endif // !CLIENT_H
