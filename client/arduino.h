/*
 * This class manages interactions with the Arduino.
 */

#ifndef ARDUINO_H
#define ARDUINO_H

#include <QtSerialPort/QSerialPort>

#include "buffergfx.h"
#include "clientgamestate.h"
#include "types.h"

class Arduino : public QObject
{
	Q_OBJECT

public:
	Arduino(ClientGameState &cgs, QObject *parent = Q_NULLPTR);

	/*
	 * Tries to connect to the arduino. Returns -1 if already connected,
	 * -2 if no device is found, and -3 if another error occurs (in which
	 * case errorOccurred will be called with a details on the failure).
	 * Returns 0 on success.
	 */
	int connectToArduino();

public slots:
	void renderLauncher();
	void renderWaiting();
	void renderTick();
	void renderGameOver(score_t score, quint16 total);

signals:
	void errorOccurred(QSerialPort::SerialPortError error, QString msg);
	void disconnected();

	void buttonA();
	void buttonB();

private slots:
	void readData();

private:
	QSerialPort *ctc;
	bool started;
	quint32 ba;
	quint32 bb;

	BufferGFX gfx;
	ClientGameState &cgs;

	void sendBuffer();
};

#endif // !ARDUINO_H
