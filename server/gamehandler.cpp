/*
 * Implements GameHandler.
 */

#include "gamehandler.h"
#include "gamelogic.h"
#include "paperserver.h"

gid_t GameHandler::idCount = 0;

GameHandler::GameHandler(PaperServer &pss, pos_t w, pos_t h, int ti, int mp, QObject *parent)
	: id(idCount)
	, width(w)
	, height(h)
	, tickInterval(ti)
	, playerCount(mp)
	, ps(pss)
	, tickTimer(new QTimer(this))
	, players()
	, grwl()
	, gs(w, h)
{
	GameHandler::idCount++;

	tickTimer->setInterval(tickInterval);
	connect(tickTimer, &QTimer::timeout, this, &GameHandler::tick);
}

void GameHandler::tick()
{
	grwl.lockForWrite();

	if (gs.players.size() < playerCount)
	{
		std::vector<std::pair<pos_t, pos_t> > spawns = findSpawns(playerCount - gs.players.size());
		/*QList<ClientHandler *> clients = ps.dequeueClients(spawns.size());
		for (auto siter = spawns.begin(), citer = clients.begin(); citer < spawns.end() && citer < clients.end(); iter++)
		{
			
		}*/
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
