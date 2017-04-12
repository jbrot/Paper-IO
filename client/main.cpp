/*
 * This is the main entry point for the paper-io client. 
 */

#include <QApplication>

#include "client.h"
#include "font.h"
#include "protocol.h"

void registerPackets();

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QGuiApplication::setApplicationDisplayName(Client::tr("Arduino-IO"));
	QCoreApplication::setOrganizationName("Delta Epsilon");
	QCoreApplication::setOrganizationDomain("dank.meeeeee.me");
	QCoreApplication::setApplicationName("Arduino-IO");

	// We need to do this so we can communicate errors across threads.
	qRegisterMetaType<QAbstractSocket::SocketError>();
	qRegisterMetaType<score_t>("score_t");
	qRegisterMetaType<Direction>("Direction");

	registerPackets();

	QString style = R"css(
QWidget
{
	background-color: #333;
	color: #FFF;
}

QPushButton
{ 
	background-color: #FC6;
	border-radius: 0px;
	border-style: solid;
	border-color: #F60;
	border-bottom-width: 8px;
	font-size: 40px;
	font-weight: 600;
	color: #F60;
	padding: 6px 0.8em;
}

QMessageBox QPushButton
{
	font-size: 20px;
}

QPushButton:hover
{
	background-color: #FC0;
}

QPushButton:pressed
{
	background-color: #F90;
}

QPushButton:disabled
{
	background-color: #d9bf8c;
	color: #bf7340;
	border-color: #bf7340;
}

QLineEdit
{
	background-color: #FFE5CC;
	border-radius: 0px;
	border-style: solid;
	border-color: #A6683F;
	border-bottom-width: 8px;
	font-size: 40px;
	font-weight: 200;
	color: #603920;
	padding: 6px;
}

QLabel
{
	font-size: 15px;
}

	)css";

	app.setStyleSheet(style);
	QApplication::setFont(getDejaVuFont());

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
