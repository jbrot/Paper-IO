/*
 * This is the main entry point for the paper-io client. 
 */

#include <QApplication>

#include "client.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QGuiApplication::setApplicationDisplayName(Client::tr("Arduino-IO"));
	QCoreApplication::setOrganizationName("Delta Epsilon");
	QCoreApplication::setOrganizationDomain("dank.meeeeee.me");
	QCoreApplication::setApplicationName("Arduin-IO");

	Client client;
	client.show();

	return app.exec();
}
