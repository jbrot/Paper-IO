/*
 * Implements GameHandler.
 */

#include "gamehandler.h"
#include "gamelogic.h"
#include "paperserver.h"

gid_t GameHandler::idCount = 0;

GameHandler::GameHandler(PaperServer &pss, pos_t w, pos_t h, quint16 ti, plid_t mp, QObject *parent)
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
	, gs(w, h, ti)
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
	qDebug() << "Game" << id << ": Tick" << gs.getTick();

	// First update AIs.
	tickAIs();

	// Now we are ready to begin the tick.
	gs.lockForWrite();

	gs.nextTick();

	qDebug() << "Game" << id << ": Player number" << gs.players.size();

	// Run the core game logic.
	updateGame(gs);

	// Check for dead players and remove.
	removePlayers();

	// Spawn new players if needed.
	if (gs.players.size() < playerCount)
		spawnPlayers();

	// If we have no players left, quit.
	if (players.size() == 0)
	{
		gs.unlock();

		qDebug() << "Game" << id << ": No more players. Terminating...";
		tickTimer->stop();
		emit terminated();
		return;
	}

	// TODO Work out leaderboard.

	gs.unlock();

	emit tickComplete();
}

void GameHandler::tickAIs()
{
	gs.lockForRead();
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

		pl->newDir = iter.value()->tick(gs);

		iter++;
	}
	gs.unlock();
}

void GameHandler::spawnPlayers()
{
	qDebug() << "Game" << id << ": Spawning...";

	std::vector<std::pair<pos_t, pos_t> > spawns = findSpawns(playerCount - gs.players.size(), gs);
	QList<QPair<ClientHandler *, QString>> clients = ps.dequeueClients(spawns.size());

	qDebug() << "Game" << id <<": Found" << spawns.size() << "locations and" << clients.size() << "players.";

	auto siter = spawns.begin();
	auto citer = clients.begin();
	for (; siter < spawns.end() && citer < clients.end(); siter++, citer++)
	{
		ClientHandler *ch = citer->first;
		if (!ch)
		{
			qWarning() << "Game" << id << ": dequeueClients retval included NULL.";
			siter--;
			continue;
		}

		if (!gs.addPlayer(currentId, citer->second, siter->first, siter->second))
		{
			citer--;
			continue;
		}

		QMetaObject::invokeMethod(ch, "beginGame", Q_ARG(plid_t, currentId), Q_ARG(GameState *, &gs));
		connect(this, &GameHandler::tickComplete, ch, &ClientHandler::sendTick);
		// We need this indirection so we can capture the id in the lambda without
		// it changing when additional clients are registered.
		plid_t pid = currentId;
		connect(ch, &ClientHandler::disconnected, this, [this, pid] {
			playerDisconnected(pid);
		});
		connect(ch, &ClientHandler::changeDirection, this, [this, pid] (Direction dir) {
			playerMoved(pid, dir);
		});

		players.insert(currentId, ch);
		configureSpawn(gs.lookupPlayer(currentId), gs);

		findNextId();
	}
	for (; siter < spawns.end(); siter++)
	{
		// TODO Add name
		if (!gs.addPlayer(currentId, QLatin1String("AI"), siter->first, siter->second))
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
		if (!citer->first)
		{
			qWarning() << "Game" << id << ": dequeueClients retval included NULL.";
			continue;
		}

		QMetaObject::invokeMethod(&ps, "queueConnection", Q_ARG(thid_t, citer->first->getId()));
	}
}

void GameHandler::findNextId()
{
	while (gs.players.contains(currentId) || currentId == UNOCCUPIED 
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
				disconnect(this, 0, ch, 0);
				disconnect(ch, 0, this, 0);
				QMetaObject::invokeMethod(ch, "endGame", Q_ARG(score_t, pl->getScore()));
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
	gs.lockForRead();
	Player *pl = gs.lookupPlayer(pid);
	if (pl)
		pl->dead = true;
	else
		qWarning() << "Game" << id << ": Received disconnect change from unregistered player " << pid << "!";
	gs.unlock();

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
	gs.lockForRead();
	Player *pl = gs.lookupPlayer(pid);
	if (pl)
		pl->newDir = dir;
	else
		qWarning() << "Game" << id << ": Received direction change from unregistered player " << pid << "!";
	gs.unlock();
}

void GameHandler::startGame()
{
	tickTimer->start();
}
