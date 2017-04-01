/*
 * The GameHandler class manages a game. The actual game logic is done via the
 * functions provided in gamelogic.h. This class primarily deals with making
 * sure everyone is on the same page (i.e., AIs, Players, and the aforementioned
 * functions).
 */

#include "types.h"

/* Game ID */
typedef quint32 gid_t;

class GameHandler : public QObject
{
	Q_OBJECT

public:
	gid_t getId() const; 

public signals:
	void terminated();

private slots:
	void tick();

	void playerDisconnected(plid_t pl);

private:
	static gid_t idCount;
	
	const gid_t id;
	QTimer tickTimer;
	QHash<plid_t, ClientHandler> players;
}
