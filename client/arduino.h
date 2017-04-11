/*
 * This class manages interactions with the Arduino.
 */

#ifndef ARDUINO_H
#define ARDUINO_H

#include <QtSerialPort/QSerialPort>

class Arduino : public QObject
{
	Q_OBJECT

public:
	Arduino(QObject *parent = Q_NULLPTR);

	/*
	 * Tries to connect to the arduino. Returns -1 if already connected,
	 * -2 if no device is found, and -3 if another error occurs (in which
	 * case errorOccurred will be called with a details on the failure).
	 * Returns 0 on success.
	 */
	int connectToArduino();

signals:
	void errorOccurred(QSerialPort::SerialPortError error, QString msg);

private slots:
	void readData();

private:
	quint8 screen[16][32];

	QSerialPort *ctc;
};

#endif // !ARDUINO_H
