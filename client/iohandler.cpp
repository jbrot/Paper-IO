/*
 * This is the implementation of the IOHandler class. This
 * is where the nitty gritty network details live.
 */

#include "iohandler.h"
#include "protocol.h"

// We initialize the socket here with this object as its parent.
// That way, when moved to our own thread, the socket will be moved
// with us. The QDataStream is not a QObject, so as long as we're
// careful about which thread we use it from, we should be fine.
IOHandler::IOHandler(ClientGameState &cg, QObject *parent)
	: QObject(parent)
	, socket(new QTcpSocket(this))
	, keepAlive(new QTimer(this))
	, name(QLatin1String(""))
	, cgs(cg)
{
	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	keepAlive->setInterval(5000);
	connect(keepAlive, &QTimer::timeout, this, &IOHandler::kaTimeout);

	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
	        this, &IOHandler::ierror);
	connect(socket, &QAbstractSocket::connected, this, &IOHandler::connected);
	connect(socket, &QAbstractSocket::connected, keepAlive, static_cast<void (QTimer::*)()>(&QTimer::start));
	connect(socket, &QAbstractSocket::connected, this, [this] {
		this->lastka = QDateTime::currentDateTime();
	});
	connect(socket, &QAbstractSocket::disconnected, this, &IOHandler::disconnected);
	connect(socket, &QAbstractSocket::disconnected, keepAlive, &QTimer::stop);
	connect(socket, &QIODevice::readyRead, this, &IOHandler::newData);
}

void IOHandler::connectToServer(const QString &host, quint16 port, const QString &nm)
{
	// The abort call is kind of overkill, but it's better to be safe than sorry
	socket->abort();
	socket->connectToHost(host, port);

	name = nm;
}

void IOHandler::abort()
{
	socket->abort();
	keepAlive->stop();
}

void IOHandler::disconnect()
{
	socket->disconnectFromHost();
}

void IOHandler::ierror(QAbstractSocket::SocketError err)
{
	emit error(err, socket->errorString());
}

void IOHandler::enterQueue()
{
	Packet::writePacket(str, PacketRequestJoin(name));
}

void IOHandler::changeDirection(Direction dir)
{
	Packet::writePacket(str, PacketUpdateDir(dir));
}

void IOHandler::requestResend()
{
	Packet::writePacket(str, PacketRequestResend());
}

void IOHandler::kaTimeout()
{
	if (lastka.secsTo(QDateTime::currentDateTime()) > TIMEOUT_LEN)
	{
		disconnect();
		qDebug() << "Haven't received keep alive packet, timing out client.";
		return;
	}
	Packet::writePacket(str, PacketKeepAlive());
	qDebug() << "Sent keep alive!";
}

void IOHandler::updatePlayerPositions()
{
	foreach (ClientPlayer *pl, cgs.players)
	{
		if (!pl)
			continue;
		pl->setX(OUT_OF_VIEW);
		pl->setY(OUT_OF_VIEW);
	}

	for (pos_t y = -(CLIENT_FRAME / 2); y < CLIENT_FRAME / 2; y++)
	{
		for (pos_t x = -(CLIENT_FRAME / 2); x < CLIENT_FRAME / 2; x++)
		{
			ClientPlayer *cp = cgs.getState(x,y).getOccupyingPlayer();
			if (!cp)
				continue;

			cp->setX(x);
			cp->setY(y);
		}
	}
}

void IOHandler::processPlayersUpdate(const PacketPlayersUpdate &ppu, bool nested)
{
	if (!nested)
		cgs.lockState();

	if (cgs.getTick() != ppu.getTick())
		qWarning() << "PPU Packet is on tick" << ppu.getTick() << ", but we're on tick" << cgs.getTick() << "!";

	QHash<plid_t, QString> players = ppu.getPlayers();

	for (auto iter = cgs.players.begin(); iter != cgs.players.end(); )
	{
		if (!players.contains(iter.key()) || (iter.value() && iter.value()->getName() != players.value(iter.key())))
		{
			delete iter.value();
			iter = cgs.players.erase(iter);
		} else {
			players.remove(iter.key());
			iter++;
		} 
	}

	for (auto iter = players.cbegin(); iter != players.cend(); iter++)
		cgs.players.insert(iter.key(), new ClientPlayer(cgs, iter.key(), iter.value(), OUT_OF_VIEW, OUT_OF_VIEW));

	if (!nested)
	{
		updatePlayerPositions();
		cgs.unlock();
	}
}

void IOHandler::processLeaderboardUpdate(const PacketLeaderboardUpdate &plu, bool nested)
{
	if (!nested)
		cgs.lockState();

	if (cgs.getTick() != plu.getTick())
		qWarning() << "PLU Packet is on tick" << plu.getTick() << ", but we're on tick" << cgs.getTick() << "!";

	const quint8 *lb = plu.getLeaderboard();

	std::copy(lb, lb + 10, cgs.leaderboard);

	for (int i = 0; i < 10; i += 2)
	{
		ClientPlayer *cp = cgs.lookupPlayer(lb[i]);
		if (!cp)
			continue;

		cp->setScore(lb[i + 1]);
	}

	if (!nested)
		cgs.unlock();
}

void IOHandler::processFullBoard(const PacketResendBoard &prb, bool nested)
{
	if (!nested)
		cgs.lockState();

	if (cgs.getTick() != prb.getTick())
		qWarning() << "PRB Packet is on tick" << prb.getTick() << ", but we're on tick" << cgs.getTick() << "!";

	const state_t *const *board = prb.getBoard();
	state_t *ptrs[CLIENT_FRAME];
	for (int i = 0; i < CLIENT_FRAME; i++)
	{
		std::copy(board[i], board[i] + CLIENT_FRAME, cgs.board[i]);
		ptrs[i] = cgs.board[i];
	}

	QByteArray chksum = hashBoard(ptrs);
	if (chksum != prb.getChecksum())
	{
		qWarning() << "PRB Checksum:" << prb.getChecksum() << "disagrees with computed:" << chksum << "! Requesting resend...";
		requestResend();
	}

	if (!nested)
	{
		updatePlayerPositions();
		cgs.unlock();
	}
}

void IOHandler::processJoinGame(const PacketGameJoin &pgj)
{
	cgs.lockState();

	cgs.client = pgj.getId();
	qDebug() << "Id" << cgs.client;
	cgs.tick = pgj.getPPU().getTick();
	qDebug() << "Tick" << cgs.tick;
	
	processPlayersUpdate(pgj.getPPU(), true);
	processLeaderboardUpdate(pgj.getPLU(), true);
	processFullBoard(pgj.getPRB(), true);
	updatePlayerPositions();

	if (!cgs.getClient())
		qWarning() << "PGJ: No client player set up!";
	else
		cgs.getClient()->setScore(pgj.getScore());

	cgs.unlock();
}

void IOHandler::processGameTick(const PacketGameTick &pgt)
{
	cgs.lockState();

	if (cgs.getTick() >= pgt.getTick())
	{
		// This is most likely the first tick.
		qWarning() << "PGT: New tick" << pgt.getTick() << "is not larger than current tick:" << cgs.getTick() << ". Ignoring.";
		cgs.unlock();
		return;
	}

	cgs.tick = pgt.getTick();
	qDebug() << "Tick:" << cgs.getTick();

	if (!cgs.getClient())
		qWarning() << "PGT: No client player set up!";
	else
		cgs.getClient()->setScore(pgt.getScore());

	// TODO We can use shifting indices in GameState to make this really fast.
	// But I don't think it matters right now.
	const state_t *news = pgt.getNewSection();
	const state_t *const *diff = pgt.getDiff();
	switch(pgt.getDirection())
	{
	case UP:
		std::copy_backward(cgs.board[0], cgs.board[CLIENT_FRAME- 1], cgs.board[1]);
		std::copy(news, news + CLIENT_FRAME, cgs.board[0]);
		for (int i = 1; i < CLIENT_FRAME; i++)
			for (int j = 0; j < CLIENT_FRAME; j++)
				cgs.board[i][j] ^= diff[i][j];
		break;
	case DOWN:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
		std::copy(cgs.board[1], cgs.board[CLIENT_FRAME], cgs.board[0]);
#pragma GCC diagnostic pop
		std::copy(news, news + CLIENT_FRAME, cgs.board[CLIENT_FRAME - 1]);
		for (int i = 0; i < CLIENT_FRAME - 1; i++)
			for (int j = 0; j < CLIENT_FRAME; j++)
				cgs.board[i][j] ^= diff[i][j];
		break;
	case LEFT:
		for (int i = 0; i < CLIENT_FRAME; i++)
		{
			std::copy_backward(&cgs.board[i][0], &cgs.board[i][CLIENT_FRAME - 1], &cgs.board[i][1]); 
			cgs.board[i][0] = news[i];
			for (int j = 1; j < CLIENT_FRAME; j++)
				cgs.board[i][j] ^= diff[i][j];
		}
		break;
	case RIGHT:
		for (int i = 0; i < CLIENT_FRAME; i++)
		{
			std::copy(&cgs.board[i][1], &cgs.board[i][CLIENT_FRAME], &cgs.board[i][0]); 
			cgs.board[i][CLIENT_FRAME - 1] = news[i];
			for (int j = 0; j < CLIENT_FRAME - 1; j++)
				cgs.board[i][j] ^= diff[i][j];
		}
		break;
	case NONE:
		for (int i = 0; i < CLIENT_FRAME; i++)
		{
			for (int j = 0; j < CLIENT_FRAME; j++)
				cgs.board[i][j] ^= diff[i][j];
		}
		break;
	}

	state_t *ptrs[CLIENT_FRAME];
	for (int i = 0; i < CLIENT_FRAME; i++)
		ptrs[i] = cgs.board[i];

	QByteArray chksum = hashBoard(ptrs);
	if (chksum != pgt.getChecksum())
	{
		qWarning() << "PGT Checksum:" << pgt.getChecksum() << "disagrees with computed:" << chksum << "! Requesting resend...";
		requestResend();
	}

	cgs.unlock();
}

void IOHandler::newData()
{
	Packet *packet = NULL;

	// Read all available packets.
	while (true)
	{
		str.startTransaction();
		packet = Packet::readPacket(str);
		if (!str.commitTransaction())
		{
			if (str.status() == QDataStream::ReadPastEnd)
				str.resetStatus();
			return;
		}
		if (!packet)
			continue;

		switch (packet->getId()) {
		case PACKET_KEEP_ALIVE:
			lastka = QDateTime::currentDateTime();
			qDebug() << "Keep alive received!";
			break;
		case PACKET_QUEUED:
			emit queued();
			break;
		case PACKET_PLAYERS_UPDATE:
			processPlayersUpdate(*static_cast<PacketPlayersUpdate *>(packet));
			break;
		case PACKET_LEADERBOARD_UPDATE:
			processLeaderboardUpdate(*static_cast<PacketLeaderboardUpdate *>(packet));
			break;
		case PACKET_RESEND_BOARD:
			processFullBoard(*static_cast<PacketResendBoard *>(packet));
			break;
		case PACKET_GAME_JOIN:
			processJoinGame(*static_cast<PacketGameJoin *>(packet));
			break;
		case PACKET_GAME_TICK:
			processGameTick(*static_cast<PacketGameTick *>(packet));
			break;
		case PACKET_GAME_END:
			emit gameEnded(static_cast<PacketGameEnd *>(packet)->getScore());
			break;
		default:
			qDebug() << "Received unknown packet: " << packet;
			break;
		}

		delete packet;
	}
}
