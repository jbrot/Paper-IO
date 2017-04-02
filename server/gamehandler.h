/*
 * The GameHandler class manages a game. The actual game logic is done via the
 * functions provided in gamelogic.h. This class primarily deals with making
 * sure everyone is on the same page (i.e., AIs, Players, and the aforementioned
 * functions).
 */

#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include <QHash>
#include <QReadWriteLock>
#include <QTimer>

#include "clienthandler.h"
#include "gamestate.h"
#include "types.h"

class PaperServer;

/* Game ID */
typedef quint32 gid_t;

class GameHandler : public QObject
{
	Q_OBJECT

public:
	/*
	 * WARNING: This constructor is NOT thread safe. Make
	 * sure you only construct one GameHandler at a time.
	 */
	GameHandler(PaperServer &server, pos_t width = 80, pos_t height = 80,
	            int tickInterval = 250, int playerCount = 10, QObject *parent = Q_NULLPTR);

	gid_t getId() const; 

signals:
	void terminated();

public slots:
	void startGame();

private slots:
	void tick();

	void playerDisconnected(plid_t pl);
	void playerMoved(plid_t id, Direction dir);

private:
	static gid_t idCount;
	
	const gid_t id;
	const pos_t width;
	const pos_t height;
	const int tickInterval;
	const int playerCount;

	PaperServer &ps;

	QTimer *tickTimer;
	QHash<plid_t, ClientHandler *> players;

	QReadWriteLock grwl;
	GameState gs;
};

#endif // !GAMEHANDLER_H
