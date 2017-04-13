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

	void enableArduino();
	void disableArduino();

	void setStatus(const QString &msg);

	/*
	 * This can be used to simulate a connect press.
	 */
	void doConnect();

private slots:
	void toggleConnect();

signals:
	void connectToServer(const QString &host, quint16 port, const QString &name);
	void connectToArduino();

private:
	QLabel *status;
	QLineEdit *nameEdit;
	QLineEdit *ipEdit;
	QLineEdit *portEdit;
	QPushButton *ctc;
	QPushButton *ard;

	bool enabled;
	bool ctenabled;
	bool aenabled;
};

#endif // !LAUNCHER_H
