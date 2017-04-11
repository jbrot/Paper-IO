#include <QtGlobal>
#include <QtSerialPort/QSerialPortInfo>

#include "arduino.h"

Arduino::Arduino(QObject *parent)
	: QObject(parent)
	, ctc(new QSerialPort(this))
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
	connect(ctc, &QSerialPort::errorOccurred, this, [ctc] (QSerialPort::SerialPortError error) {
#endif
		emit errorOccurred(error, this->ctc->errorString());
	} );
}

int Arduino::connectToArduino()
{
	if (ctc->isOpen())
		return -1;

	const auto infos = QSerialPortInfo::availablePorts();
	QSerialPortInfo ainf;
	bool found = false;
	for (const QSerialPortInfo &info : infos) {
		if (!info.manufacturer().startsWith("Arduino"))
			continue;

		// Linux and OS X have two devices. We want the cu one.
#ifndef _WIND32
		if (!info.portName().startsWith("cu."))
			continue;
#endif

		ainf = info;
		found = true;
		break;
	}

	if (!found)
		return -2;

	if (!ctc->open(QIODevice::ReadWrite))
		return -3;
	
	return 0;
}

void Arduino::readData()
{
	char data = 0;
	while (ctc->getChar(&data))
	{
		// TODO process input
	}
}
