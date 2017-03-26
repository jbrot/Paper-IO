/*
 * Implements PaperIOSever, which listens for new connections and assigns them
 * to a game.
 */

#include "PaperServer.h"

PaperServer::PaperServer(QObject *parent) : QTcpServer(parent)
{
	// Nothing to do yet
}

void PaperServer::incomingConnection(qintptr socketDescriptor)
{
	// TODO we should probably do something here
}
