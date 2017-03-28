/*
 * This is the main entry point for the paper-io server. A lot of this
 * code is adapted from the FortuneServer example.
 */

#include <QApplication>
#include <QtNetwork>

#include "paperserver.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	PaperServer server;

	if (!server.listen()) {
		qCritical() << "Unable to start server: " << server.errorString();
		return server.serverError();
	}

	// Log to the console where we're listening
	quint16 port = server.serverPort();

	QList<QHostAddress> ips = QNetworkInterface::allAddresses();
	if (!ips.size()) {
		qWarning() << "No IPs detected. Listening on port " << port;
	}
	for (int i = 0; i < ips.size(); i++) {
		qInfo() << "Listening at: " << ips.at(i).toString() << " on port " << port;
	}

	return app.exec();
}
