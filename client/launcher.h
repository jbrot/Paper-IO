/*
 * This class provides the Launcher QWidget which provides
 * the initial GUI before the connection is established.
 */

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class Launcher : public QWidget
{
	Q_OBJECT

public:
	Launcher(QWidget *parent = 0);

public slots:
	void enable();
	void disable();

	void enableConnect();
	void disableConnect();

	void setStatus(const QString &msg);

private slots:
	void doConnect();
	void toggleConnect();

signals:
	void connectToServer(const QString &host, quint16 port, const QString &name);

private:
	QLabel *status;
	QLineEdit *nameEdit;
	QLineEdit *ipEdit;
	QLineEdit *portEdit;
	QPushButton *ctc;

	bool enabled;
	bool ctenabled;
};

#endif // !LAUNCHER_H
