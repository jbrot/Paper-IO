/*
 * Implements GameHandler.
 */

#include "gamehandler.h"
#include "gamelogic.h"
#include "paperserver.h"

gid_t GameHandler::idCount = 0;

GameHandler::GameHandler(PaperServer &pss, pos_t w, pos_t h, int ti, plid_t mp, QObject *parent)
	: QObject(parent)
	, id(idCount)
	, width(w)
	, height(h)
	, tickInterval(ti)
	, playerCount(mp)
	, ps(pss)
	, tickTimer(new QTimer(this))
	, players()
	, ais()
	, currentId(1)
	, grwl()
	, gs(w, h)
{
	GameHandler::idCount++;

	tickTimer->setInterval(tickInterval);
	connect(tickTimer, &QTimer::timeout, this, &GameHandler::tick);
}

GameHandler::~GameHandler()
{
	foreach (AIPlayer *aip, ais)
		delete aip;
}

gid_t GameHandler::getId() const
{
	return id;
}

void GameHandler::tick()
{
	// First update AIs.
	tickAIs();

	// Now we are ready to begin the tick.
	grwl.lockForWrite();

	gs.nextTick();

	// Spawn new players if needed.
	if (gs.players.size() < playerCount)
		spawnPlayers();

	// Run the core game logic.
	updateGame(gs);

	// Check for dead players and remove.
	removePlayers();

	grwl.unlock();

	// If we have no players left, quit.
	if (players.size() == 0)
	{
		tickTimer->stop();
		emit terminated();
	} else {
		emit tickComplete();
	}
}

void GameHandler::tickAIs()
{
	grwl.lockForRead();
	auto iter = ais.begin();
	while(iter != ais.end())
	{
		plid_t pid = iter.key();
		Player *pl = gs.lookupPlayer(pid);
		if (!pl || !iter.value())
		{
			qWarning() << "Game" << id << ": AI" << pid << "is either not in game or NULL! Deleting...";
			if (iter.value())
				delete iter.value();
			iter = ais.erase(iter);
			continue;
		}

		pos_t x = pl->getX(), y = pl->getY();
		ROGameState rgs(x, y, gs);
		pl->newDir = iter.value()->tick(rgs);

		iter++;
	}
	grwl.unlock();
}

void GameHandler::spawnPlayers()
{
	std::vector<std::pair<pos_t, pos_t> > spawns = findSpawns(playerCount - gs.players.size(), gs);
	QList<ClientHandler *> clients = ps.dequeueClients(spawns.size());

	auto siter = spawns.begin();
	auto citer = clients.begin();
	for (; siter < spawns.end() && citer < clients.end(); siter++, citer++)
	{
		if (!*citer)
		{
			qWarning() << "Game" << id << ": dequeueClients retval included NULL.";
			siter--;
			continue;
		}

		if (!gs.addPlayer(currentId, siter->first, siter->second))
		{
			citer--;
			continue;
		}

		// TODO This might need set up before it can work.
		QMetaObject::invokeMethod(*citer, "beginGame", Q_ARG(plid_t, currentId),
		                          Q_ARG(GameState *, &gs), Q_ARG(QReadWriteLock, grwl));
		connect(this, &GameHandler::tickComplete, *citer, &ClientHandler::sendTick);

		players.insert(currentId, *citer);
		configureSpawn(gs.lookupPlayer(currentId), gs);

		findNextId();
	}
	for (; siter < spawns.end(); siter++)
	{
		if (!gs.addPlayer(currentId, siter->first, siter->second))
			continue;

		AIPlayer *ai = new AIPlayer();
		ais.insert(currentId, ai);
		configureSpawn(gs.lookupPlayer(currentId), gs);

		findNextId();
	}

	if (citer == clients.end())
		return;

	qWarning() << "Game" << id << ": Did not place all requested players. Requeuing...";
	for (; citer < clients.end(); citer++)
	{
		if (!*citer)
		{
			qWarning() << "Game" << id << ": dequeueClients retval included NULL.";
			continue;
		}

		QMetaObject::invokeMethod(&ps, "queueConnection", Q_ARG(thid_t, (*citer)->getId()));
	}
}

void GameHandler::findNextId()
{
	while (players.contains(currentId) || ais.contains(currentId) 
	                                   || currentId == UNOCCUPIED 
	                                   || currentId == OUT_OF_BOUNDS)
		currentId++;
}

void GameHandler::removePlayers()
{
	for (auto iter = gs.players.begin(); iter != gs.players.end(); )
	{
		Player *pl = iter.value();
		if (!pl)
		{
			qWarning() << "Game" << id << ": Player" << iter.key() << "is NULL!";
			iter++;
			continue;
		}

		if (!pl->isDead())
		{
			iter++;
			continue;
		}

		if (ais.contains(iter.key()))
		{
			AIPlayer *apl = ais.value(iter.key());
			ais.remove(iter.key());
			if (!apl)
				qWarning() << "Game" << id << ": AI" << iter.key() << "is NULL!";
			else
				delete apl;
		} else if (players.contains(iter.key())) {
			ClientHandler *ch = players.value(iter.key());
			players.remove(iter.key());
			if (!ch)
			{
				qWarning() << "Game" << id << ": Client" << iter.key() << "is NULL!";
			} else {
				disconnect(this, &GameHandler::tickComplete, ch, &ClientHandler::sendTick);
				QMetaObject::invokeMethod(ch, "endGame", Q_ARG(quint8, pl->getScore()));
			}
		} else {
			qWarning() << "Game" << id << ": Player" << iter.key() << "is neither an AI nor a Player!";
		}

		iter = gs.removePlayer(iter);
	}
}

void GameHandler::playerDisconnected(plid_t pid)
{
	// We are just going to set the dead flag without actually
	// killing the player. This flag is a server side only one
	// so we don't need to lockForWrite to change it.
	grwl.lockForRead();
	Player *pl = gs.lookupPlayer(pid);
	if (pl)
		pl->dead = true;
	else
		qWarning() << "Game" << id << ": Received disconnect change from unregistered player " << pid << "!";
	grwl.unlock();

	// Since the player has disconnected, the connection object is invalid
	// so we need to remove it immediately.
	if (!players.remove(pid))
		qWarning() << "Game" << id << ": Tried to release Player" << pid << "but didn't have connection!";
}

void GameHandler::playerMoved(plid_t pid, Direction dir)
{
	// Although we are making changes, we are only changing the newDir value
	// of the player which is never sent to the client---it is only read by
	// the game logic when advancing ticks. So, securing a readlock is enough
	// as it prevents writes which might invalidate the player while not
	// kicking out all of the IO threads.
	grwl.lockForRead();
	Player *pl = gs.lookupPlayer(pid);
	if (pl)
		pl->newDir = dir;
	else
		qWarning() << "Game" << id << ": Received direction change from unregistered player " << pid << "!";
	grwl.unlock();
}

void GameHandler::startGame()
{
	tickTimer->start();
}
