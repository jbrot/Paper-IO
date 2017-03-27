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
#include <QTcpSocket>
#include <QWidget>

class Client : public QWidget
{
	Q_OBJECT

public:
	Client(QWidget *parent = 0);

	QSize sizeHint() const override;

private slots:
	void displayError(QAbstractSocket::SocketError socketError);
	void displayError2(QNetworkSession::SessionError sessionError);
	void sessionOpened();
	void connectToServer();
	void enableConnect();

private:
	QLabel *status;
	QLineEdit *nameEdit;
	QLineEdit *ipEdit;
	QLineEdit *portEdit;
	QPushButton *ctc;

	QTcpSocket *socket;
	QDataStream str;

	QNetworkSession *session;
};

#endif // !CLIENT_H
