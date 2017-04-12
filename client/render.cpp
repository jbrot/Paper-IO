/*
 * This file includes the actual rendering code.
 */

#include <QtCore>

#include "render.h"

//background color
static QBrush background = QBrush(QColor(200, 200, 200));

//player colors
static QColor playerColors[] ={QColor(220, 60, 60),
                                 QColor(255, 0, 0),
                                 QColor(0, 255, 0),
                                 QColor(0, 0, 255),
                                 QColor(0, 238, 118),
                                 QColor(255, 255, 0),
                                 QColor(255, 165, 0),
                                 QColor(128, 0, 128),
                                 QColor(176, 226, 255),
                                 QColor(156, 102, 31),
                                 QColor(255, 153, 18),
                                 QColor(105, 105, 105)};
const int NUM_COLORS = sizeof(playerColors);

QHash<plid_t, int> colorMap;

void updateColorMap(QList<ClientPlayer *> players)
{

    for (auto iter = colorMap.begin(); iter != colorMap.end(); )
    {
        for (auto pter = players.begin(); pter < players.end(); ++pter)
        {
            if (iter.key() == (*pter)->getId())
            {
                //players.erase(pter);
                ++iter;
                goto first_loop_continue;
            }
        }
       //this statement only executes if a player is assigned a color, but has lost
        iter = colorMap.erase(iter);

    first_loop_continue: ;
    }
    //any player left in players is a new player, not assigned a color but in game
    if (players.empty())
        return;

    auto pter = players.cbegin();

    for (int i = 0; i < NUM_COLORS; ++i)
    {
        for (auto iter = colorMap.cbegin(); iter != colorMap.cend(); ++iter)
        {
            if (i == iter.value())
               goto second_loop_continue;
        }

        colorMap.insert((*pter)->getId(), i);

        if (++pter >= players.cend())
            return;

    second_loop_continue: ;
    }

    qCritical() << "Error: Could not assign a color to all players!";

}

void renderGame(ClientGameState &cgs, QPainter *painter, QPaintEvent *event)
{
    QRect rect = event->rect();
    const int CENTER_X = rect.x() + rect.width()/2;
    const int CENTER_Y = rect.y() + rect.height()/2;

    const int SQUARE_SIZE = std::max(std::max(rect.height()/31, rect.width()/31), 60);
    const int OUTLINE_SIZE = 7;

    const int CTOP_X = CENTER_X - 0.5 * SQUARE_SIZE;
    const int CTOP_Y = CENTER_Y - 0.5 * SQUARE_SIZE;


    updateColorMap(cgs.getPlayers());

    //printing background
	painter->fillRect(event->rect(), background);

    //define center point (player is always centered, so this will always execute with fixed points)
    QPoint CenterPt;
    CenterPt.setX(CENTER_X);
    CenterPt.setY(CENTER_Y);

    //rendering square by square
    for (int x = -15; x <= 15; ++x)
    {
        for (int y = -15; y <= 15; ++y)
        {
            ClientSquareState state = cgs.getState(x, y);
            if(state.isOwned())
            {
                painter->fillRect(CTOP_X + x * SQUARE_SIZE,
                                  CTOP_Y + y * SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  playerColors[colorMap.value(state.getOwningPlayerId())]);
            }

            if(state.hasTrail())
            {
                painter->fillRect(CTOP_X + x * SQUARE_SIZE,
                                  CTOP_Y + y * SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  playerColors[colorMap.value(state.getTrailPlayerId())]);
            }

            if(state.isOccupied())
            {
                painter->fillRect(CTOP_X + x * SQUARE_SIZE,
                                  CTOP_Y + y * SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  playerColors[colorMap.value(state.getOccupyingPlayerId())]);
            }


        }
    }

    //test!!!
    /*
       QBrush centerBrush(Qt::red, Qt::DiagCrossPattern);
       QRect testRec(10,10,100,100);
       painter->drawRect(testRec);
       painter->fillRect(testRec,centerBrush
    */




}

void renderGameArduino(ClientGameState &cgs, BufferGFX &gfx)
{
	qDebug() << "Arduino Tick!";
}
