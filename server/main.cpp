/*
 * This is the main entry point for the paper-io server. A lot of this
 * code is adapted from the FortuneServer example.
 */

#include <QApplication>
#include <QtNetwork>

#include "gamestate.h"
#include "paperserver.h"
#include "protocol.h"

void registerPackets();

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	// Queued Connection type registrations
	qRegisterMetaType<QAbstractSocket::SocketError>();
	qRegisterMetaType<GameState *>();
	qRegisterMetaType<plid_t>("plid_t");

	// Make sure the protocol is set up
	registerPackets();

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

void registerPackets()
{
	Packet::registerPacket(PACKET_KEEP_ALIVE, std::unique_ptr<APacketFactory>(new PacketFactory<PacketKeepAlive>()));
	Packet::registerPacket(PACKET_REQUEST_JOIN, std::unique_ptr<APacketFactory>(new PacketFactory<PacketRequestJoin>()));
	Packet::registerPacket(PACKET_QUEUED, std::unique_ptr<APacketFactory>(new PacketFactory<PacketQueued>()));
	Packet::registerPacket(PACKET_PLAYERS_UPDATE, std::unique_ptr<APacketFactory>(new PacketFactory<PacketPlayersUpdate>()));
	Packet::registerPacket(PACKET_LEADERBOARD_UPDATE, std::unique_ptr<APacketFactory>(new PacketFactory<PacketLeaderboardUpdate>()));
	Packet::registerPacket(PACKET_RESEND_BOARD, std::unique_ptr<APacketFactory>(new PacketFactory<PacketResendBoard>()));
	Packet::registerPacket(PACKET_GAME_JOIN, std::unique_ptr<APacketFactory>(new PacketFactory<PacketGameJoin>()));
	Packet::registerPacket(PACKET_GAME_TICK, std::unique_ptr<APacketFactory>(new PacketFactory<PacketGameTick>()));
	Packet::registerPacket(PACKET_UPDATE_DIR, std::unique_ptr<APacketFactory>(new PacketFactory<PacketUpdateDir>()));
	Packet::registerPacket(PACKET_REQUEST_RESEND, std::unique_ptr<APacketFactory>(new PacketFactory<PacketRequestResend>()));
	Packet::registerPacket(PACKET_GAME_END, std::unique_ptr<APacketFactory>(new PacketFactory<PacketGameEnd>()));
}
