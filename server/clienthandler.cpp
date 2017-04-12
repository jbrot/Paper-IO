/*
 * Implements IO for a client.
 */

#include <QHostAddress>

#include "clienthandler.h"
#include "protocol.h"

thid_t ClientHandler::idCount = 0;

ClientHandler::ClientHandler(QObject *parent)
	: QObject(parent)
	, id(idCount)
	, keepAlive(new QTimer(this))
	, socket(new QTcpSocket(this))
	, state(LIMBO)
	, player(NULL_ID)
	, name(QLatin1String(""))
{
	// Note that due to not locking this makes the constructor not
	// thread safe.
	ClientHandler::idCount++;

	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	keepAlive->setInterval(5000);
	connect(keepAlive, &QTimer::timeout, this, &ClientHandler::kaTimeout);
	lastka = QDateTime::currentDateTime();

	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
	        this, &ClientHandler::ierror);
	connect(socket, &QAbstractSocket::disconnected, this, &ClientHandler::disconnected);
	connect(socket, &QAbstractSocket::disconnected, keepAlive, &QTimer::stop);
	connect(socket, &QIODevice::readyRead, this, &ClientHandler::newData);
}

thid_t ClientHandler::getId() const
{
	return id;
}

void ClientHandler::enqueue()
{
	state = QUEUEING;
	player = NULL_ID;
	gs = NULL;

	Packet::writePacket(str, PacketQueued());
}

void ClientHandler::beginGame(plid_t pid, GameState *g)
{
	if (state != QUEUEING)
	{
		qWarning() << "Connection" << id <<": beginGame() received when not queueing!";
		return;
	}
	if (!g)
	{
		qCritical() << "Connection" << id << ": beginGame() passed NULL GameState!";
		return;
	}

	Player *pl = g->lookupPlayer(pid);
	if (!pl)
	{
		qCritical() << "Connection" << id << ": beginGame() passed player id which maps to NULL!";
		return;
	}

	state = INGAME;
	player = pid;
	gs = g;

	gs->lockForRead();
	Packet::writePacket(str, PacketGameJoin(pid, pl->getScore(), gs->getWidth() * gs->getHeight(), makePPU(), makePLU(), makePRB()));
	gs->unlock();
}

void ClientHandler::endGame(quint8 score)
{
	if (state != INGAME)
	{
		qWarning() << "Connection" << id <<": Received endGame() while not in game!";
		return;
	}

	state = LIMBO;
	player = NULL_ID;
	gs = NULL;

	Packet::writePacket(str, PacketGameEnd(score));
}

void ClientHandler::sendTick()
{
	qDebug() << "Sending tick...";
	if (state != INGAME || !gs)
	{
		qWarning() << "Connection" << id <<": Received sendTick() while not in game or with invalid game state!";
		return;
	}

	gs->lockForRead();

	Player *pl = gs->lookupPlayer(player);
	if (!pl)
	{
		qWarning() << "Connection" << id << ": Could not look up Player object!";
		gs->unlock();
		return;
	}

	pos_t px = pl->getX() - (CLIENT_FRAME / 2);
	pos_t py = pl->getY() - (CLIENT_FRAME / 2);
	pos_t my = gs->getHeight();
	pos_t mx = gs->getWidth();
	state_t news[CLIENT_FRAME];

	// Compute the new row.
	switch (pl->getActualDirection())
	{
	case UP:
		if (py < 0)
			std::copy(gs->boardStart, gs->boardStart + CLIENT_FRAME, news);
		else
			std::copy(gs->board[py] + px, gs->board[py] + px + CLIENT_FRAME, news);
		break;
	case DOWN:
		if (py + CLIENT_FRAME > my)
			std::copy(gs->boardStart, gs->boardStart + CLIENT_FRAME, news);
		else
			std::copy(gs->board[py + CLIENT_FRAME - 1] + px, gs->board[py + CLIENT_FRAME - 1] + px + CLIENT_FRAME, news);
		break;
	case LEFT:
		if (px < 0)
			std::copy(gs->boardStart, gs->boardStart + CLIENT_FRAME, news);
		else
			for (pos_t y = 0; y < CLIENT_FRAME; y++)
				news[y] = (0 <= py + y && py + y < my) ? gs->board[py + y][px] : OUT_OF_BOUNDS_STATE;
		break;
	case RIGHT:
		if (px + CLIENT_FRAME > mx)
			std::copy(gs->boardStart, gs->boardStart + CLIENT_FRAME, news);
		else
			for (pos_t y = 0; y < CLIENT_FRAME; y++)
				news[y] = (0 <= py + y && py + y < my) ? gs->board[py + y][px + CLIENT_FRAME - 1] : OUT_OF_BOUNDS_STATE;
		break;
	// When we don't move, the new data is ignored.
	case NONE:
		break;
	}

	state_t *dptrs[CLIENT_FRAME];
	state_t *bptrs[CLIENT_FRAME];
	for (int y = 0; y < CLIENT_FRAME; y++)
	{
		if (py + y < 0 || py + y >= my)
		{
			dptrs[y] = gs->diffStart;
			bptrs[y] = gs->boardStart;
		} else {
			dptrs[y] = gs->diff[py + y] + px;
			bptrs[y] = gs->board[py + y] + px;
		}
	}

	QByteArray chksum = hashBoard(bptrs);

	Packet::writePacket(str, PacketGameTick(gs->getTick(), pl->getActualDirection(), pl->getScore(), news, dptrs, chksum));

	if (gs->havePlayersChanged())
		Packet::writePacket(str, makePPU());

	if (gs->hasLeaderboardChanged())
		Packet::writePacket(str, makePLU());

	gs->unlock();
}

void ClientHandler::establishConnection(int socketDescriptor)
{
	if (!socket->setSocketDescriptor(socketDescriptor))
	{
		ierror(socket->error());
		return;
	}

	keepAlive->start();

	qDebug() << "Connection" << id << "established with:" << socket->peerAddress(); 

	emit connected();
}

void ClientHandler::abort()
{
	socket->abort();
	keepAlive->stop();
}

void ClientHandler::disconnect()
{
	socket->disconnectFromHost();
}

void ClientHandler::ierror(QAbstractSocket::SocketError err)
{
	emit error(err, socket->errorString());
}

void ClientHandler::kaTimeout()
{
	if (lastka.secsTo(QDateTime::currentDateTime()) > TIMEOUT_LEN)
	{
		qDebug() << "Connection " << id << ": Haven't received keep alive packet, timing out client.";
		disconnect();
		return;

	}
	Packet::writePacket(str, PacketKeepAlive());
	qDebug() << "Connection " << id << ": Keep alive sent!";
}

void ClientHandler::newData()
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
			qDebug() << "Connection" << id << ": Keep alive received!";
			break;
		case PACKET_REQUEST_JOIN:
		{
			QString nme = static_cast<PacketRequestJoin *>(packet)->getName();
			if (nme.isEmpty())
				nme = name;
			else
				name = nme;

			qDebug() << "Connection" << id << ": Requesting join with name:" << nme;
			emit requestJoinGame(nme);
			break;
		}
		case PACKET_UPDATE_DIR:
		{
			Direction dir = static_cast<PacketUpdateDir *>(packet)->getDirection();
			qDebug() << "Connection" << id << ": Requesting new direction:" << dir;
			emit changeDirection(dir);
			break;
		}
		case PACKET_REQUEST_RESEND:
		{
			qDebug() << "Connection" << id << ": Requesting resend!";
			if (state != INGAME || !gs)
			{
				qWarning() << "Connection" << id << ": Can't resend data because we're not in game or the game state is NULL!";
				break;
			}

			gs->lockForRead();
			Packet::writePacket(str, makePRB());
			gs->unlock();
			break;
		}
		default:
			qDebug() << "Connection" << id << ": Received unexpected packet: " << packet->getId();
			break;
		}

		delete packet;
	}
}

PacketPlayersUpdate ClientHandler::makePPU()
{
	if (state != INGAME || !gs)
	{
		qWarning() << "Connection" << id << ": Requested PacketPlayersUpdate while not in game or with invalid game state!";
		return PacketPlayersUpdate();
	}

	QHash<plid_t, QString> players;
	players.reserve(gs->players.size());
	for (auto iter = gs->players.cbegin(); iter != gs->players.cend(); iter++)
		if (iter.value())
			players.insert(iter.key(), iter.value()->getName());

	return PacketPlayersUpdate(gs->getTick(), players);
}

PacketLeaderboardUpdate ClientHandler::makePLU()
{
	if (state != INGAME || !gs)
	{
		qWarning() << "Connection" << id << ": Requested PacketLeaderboardUpdate while not in game or with invalid game state!";
		return PacketLeaderboardUpdate();
	}

	return PacketLeaderboardUpdate(gs->getTick(), gs->leaderboard);
}

PacketResendBoard ClientHandler::makePRB()
{
	if (state != INGAME || !gs)
	{
		qWarning() << "Connection" << id << ": Requested PacketResendBoard while not in game or with invalid game state!";
		return PacketResendBoard();
	}

	Player * pl = gs->lookupPlayer(player);
	if (!pl)
	{
		qWarning() << "Connection" << id << ": makePRB: GameState does not include our player:" << player;
		return PacketResendBoard();
	}

	pos_t px = pl->getX() - (CLIENT_FRAME / 2);
	pos_t py = pl->getY() - (CLIENT_FRAME / 2);
	pos_t my = gs->getHeight();
	state_t *ptrs[CLIENT_FRAME];
	for (int y = 0; y < CLIENT_FRAME; y++)
	{
		if (py + y < 0 || py + y >= my)
			ptrs[y] = gs->boardStart;
		else
			ptrs[y] = gs->board[py + y] + px;
			
	}

	return PacketResendBoard(gs->getTick(), ptrs);
}
