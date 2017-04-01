/*
 * This is the main entry point for the paper-io client. 
 */

#include <QApplication>

#include "client.h"
#include "protocol.h"

void registerPackets();

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QGuiApplication::setApplicationDisplayName(Client::tr("Arduino-IO"));
	QCoreApplication::setOrganizationName("Delta Epsilon");
	QCoreApplication::setOrganizationDomain("dank.meeeeee.me");
	QCoreApplication::setApplicationName("Arduino-IO");

	registerPackets();

	Client client;
	client.show();

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
