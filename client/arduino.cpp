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
	, ba(false)
	, bb(false)
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
#endif

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
	const uint16_t colors[3] = { BufferGFX::Color333(3,0,0)
	                           , BufferGFX::Color333(0,3,0)
	                           , BufferGFX::Color333(0,0,3)
	                           };
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

	sendBuffer();
}

void Arduino::renderWaiting()
{
	if (!ctc->isOpen())
		return;

	gfx.fillScreen(0);

	const char ctcng[] = "CONNECTING";
	gfx.setTextColor(BufferGFX::Color333(2,2,2));
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
	gfx.setTextColor(BufferGFX::Color333(2,2,2));
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
			if (!ba)
			{
				ba = true;
				if (started)
					emit buttonA();
			}
		} else {
			ba = false;
		}

		if (data & 0x02)
		{
			if (!bb)
			{
				bb = true;
				if (started)
					emit buttonB();
			}
		} else {
			bb = false;
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
