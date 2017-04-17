/*
 * Provides the details of the Arduino integration.
 */

#include <algorithm>
#include <stdio.h>
#include <QtCore>
#include <QtSerialPort/QSerialPortInfo>

#include "arduino.h"
#include "render.h"

Arduino::Arduino(ClientGameState &gs, QObject *parent)
	: QObject(parent)
	, ctc(new QSerialPort(this))
	, started(false)
	, ba(1)
	, bb(1)
	, gfx()
	, cgs(gs)
{
	ctc->setBaudRate(115200);
	ctc->setParity(QSerialPort::NoParity);
	ctc->setFlowControl(QSerialPort::NoFlowControl);
	ctc->setStopBits(QSerialPort::OneStop);
	ctc->setDataBits(QSerialPort::Data8);

	connect(ctc, &QIODevice::readyRead, this, &Arduino::readData);
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
	connect(ctc, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, [this] (QSerialPort::SerialPortError error) {
#else
	connect(ctc, &QSerialPort::errorOccurred, this, [this] (QSerialPort::SerialPortError error) {
#endif
		if (error == QSerialPort::NoError)
			return;

		emit errorOccurred(error, this->ctc->errorString());

		if (error == QSerialPort::ResourceError)
		{
			ctc->close();
			emit disconnected();
		}
	} );
}

int Arduino::connectToArduino()
{
	if (ctc->isOpen())
		return -1;

	const auto infos = QSerialPortInfo::availablePorts();
	QSerialPortInfo ainf;
	bool found = false;
	qDebug() << "Searching for Arduino...";
	for (const QSerialPortInfo &info : infos)
	{
		QString s = QObject::tr("Port: ") + info.portName() + "\n"
						   + QObject::tr("Location: ") + info.systemLocation() + "\n"
						   + QObject::tr("Description: ") + info.description() + "\n"
						   + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
						   + QObject::tr("Serial number: ") + info.serialNumber() + "\n"
						   + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
						   + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
						   + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";
		qDebug() << qPrintable(s);

		if (info.vendorIdentifier() != 0x2341 || info.productIdentifier() != 0x43)
			 continue;

		// Linux and OS X have two devices. We want the cu one.
#ifndef _WIN32
		if (!info.portName().startsWith("cu."))
			continue;
#endif // !_WIN32

		qDebug() << "Arduino found!";
		ainf = info;
		found = true;
		break;
	}

	if (!found)
		return -2;

	ctc->setPort(ainf);
	if (!ctc->open(QIODevice::ReadWrite))
		return -3;

	return 0;
}

void Arduino::renderLauncher()
{
	if (!ctc->isOpen())
		return;

	gfx.fillScreen(0);

	const char arduino[] = "ARDUINO-IO";
	const uint16_t colors[3] = {1, 2, 4};
	gfx.setCursor(1,0);
	for (int i = 0; i < 5; i++)
	{
		gfx.setTextColor(colors[i % 3]);
		gfx.write(arduino[i]);
	}

	gfx.setCursor(1,9);
	for (int i = 5; i < 10; i++)
	{
		gfx.setTextColor(colors[i % 3]);
		gfx.write(arduino[i]);
	}

/*
	// This code draws a color table. It is useful for picking colors
	// on the Arduino.
	int x, y;
	for (int c = 0; c < 255; c++)
	{
		if (c < 16)
		{
			x = c;
			y = 0;
		} else if (16 <= c && c < 232) {
			x = 7 * (((c - 16) / 36) % 3) + (c - 16) % 6;
			y = 7 * ((c - 16) / 108) + ((c - 16) % 36) / 6 + 2;
		} else {
			x = (c - 232) % 8 + 22;
			y = 3 + (c - 232) / 8;
		}
		gfx.drawPixel(x, y, c);
	}
*/

	sendBuffer();
}

void Arduino::renderWaiting()
{
	if (!ctc->isOpen())
		return;

	gfx.fillScreen(0);

	const char ctcng[] = "CONNECTING";
	gfx.setTextColor(240);
	gfx.setCursor(1,0);
	for (int i = 0; i < 5; i++)
		gfx.write(ctcng[i]);

	gfx.setCursor(1,9);
	for (int i = 5; i < 10; i++)
		gfx.write(ctcng[i]);

	sendBuffer();
}

void Arduino::renderTick()
{
	if (!ctc->isOpen())
		return;

	cgs.lockState();
	renderGameArduino(cgs, gfx);
	cgs.unlock();

	sendBuffer();
}

void Arduino::renderGameOver(score_t score, quint16 total)
{
	if (!ctc->isOpen())
		return;

	gfx.fillScreen(0);

	double pct = 100 * score / (double) total;

	char scr[] = "Score";
	gfx.setTextColor(240);
	gfx.setCursor(1,0);
	for (int i = 0; i < 5; i++)
		gfx.write(scr[i]);

	if (score == total)
	{
		scr[0] = '1';
		scr[1] = '0';
		scr[2] = '0';
		scr[3] = '%';
		scr[4] = '!';
	} else {
		pct = std::min(99.9, pct);
	}
	snprintf(scr, sizeof(scr), "%.2f", pct);
	scr[4] = '%';
	gfx.setCursor(1,9);
	for (int i = 0; i < 5; i++)
		gfx.write(scr[i]);

	sendBuffer();
}

void Arduino::readData()
{
	char data = 0;
	while (ctc->getChar(&data))
	{
		if (data & 0x01)
		{
			// We've had issues with Button A flickering between
			// 0 and 1 for a bit before stabilizing. None of the
			// intervals were over 100. Furthermore, none of the actual
			// intervals were ever under 500 as our buad rate is 115200
			// which puts 500 at just under a twentieth of a second.
			//
			// N.B. ba and bb will overflow at around 10 hours. However,
			// since they're unsigned, an overflow will simply prevent
			// the button for being registered for one tenth of a second
			// which isn't an issue at all (especially since it hasn't been
			// pressed in over 10 hours at this point)
			if (ba > 500)
			{
				ba = 0;
				if (started)
					emit buttonA();
			}
		} else {
			++ba;
		}

		if (data & 0x02)
		{
			if (bb > 500)
			{
				bb = 0;
				if (started)
					emit buttonB();
			}
		} else {
			++bb;
		}
	}
}

void Arduino::sendBuffer()
{
	started = true;

	ctc->putChar(0xFF);

	uint8_t (*buf)[ARDUINO_WIDTH] = gfx.getBuffer();
	for (int y = 0; y < ARDUINO_HEIGHT; y++)
		for (int x = 0; x < ARDUINO_WIDTH; x++)
			ctc->putChar(buf[y][x]);
}
