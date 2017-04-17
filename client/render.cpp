/*
 * This file includes the actual rendering code.
 */

#include <QtCore>
#include <QFont>

#include "render.h"

static int getXOff(Direction dir)
{
    switch (dir)
    {
    case LEFT:
        return -1;
    case RIGHT:
        return 1;
    default:
        return 0;
    }
}

static int getYOff(Direction dir)
{
    switch (dir)
    {
    case UP:
        return -1;
    case DOWN:
        return 1;
    default:
        return 0;
    }
}

// Background color
static QBrush background = QBrush(QColor(200, 200, 200));

// Player colors

static QColor playerColors[11][2] = { {QColor(0xFF99CC), QColor(0xE481C3)},  // Magenta
                                      {QColor(0xFF0033), QColor(0xCC3333)},  // Red
                                      {QColor(0x33CC33), QColor(0x009933)},  // Green
                                      {QColor(0x3333FF), QColor(0x002DB3)},  // Blue
                                      {QColor(0xFF9933), QColor(0xEC6A13)},  // Orange
                                      {QColor(0xB366FF), QColor(0x8A00E6)},  // Purple
                                      {QColor(0x00FF99), QColor(0x00CC99)},  // Eucalyptus
                                      {QColor(0x33CCFF), QColor(0x00B8E6)},  // Cyan
                                      {QColor(0xFFFF80), QColor(0xF2F20D)},  // Yellow
                                      {QColor(0xFF9999), QColor(0xFF6666)}}; // Salmon

static QColor outOfBoundsColor = QColor(100,100,100);
const int NUM_COLORS = sizeof(playerColors);

QHash<plid_t, int> colorMap;

static void updateColorMap(QList<const ClientPlayer *> players)
{
    for (auto iter = colorMap.begin(); iter != colorMap.end(); )
    {
        for (auto pter = players.begin(); pter < players.end(); ++pter)
        {
            if (iter.key() == (*pter)->getId())
            {
                players.erase(pter);
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

void renderGame(const ClientGameState &cgs, QPainter *painter, QPaintEvent *event)
{

    QRect rect = event->rect();
    const int CENTER_X = rect.x() + rect.width()/2;
    const int CENTER_Y = rect.y() + rect.height()/2;

    const int SQUARE_SIZE = std::max(std::max(ceil(rect.height()/static_cast<double>(CLIENT_FRAME - 2)),
                                              ceil(rect.width()/static_cast<double>(CLIENT_FRAME - 2))), 25.0);
    const int OUTLINE_SIZE = 7;

    int offset = SQUARE_SIZE * ((cgs.getLastTick().msecsTo(QDateTime::currentDateTime()))
                / static_cast<double>(cgs.getTickRate() + 10));
    offset = std::min(offset - SQUARE_SIZE, 0);

    const int CTOP_X = CENTER_X - 0.5 * SQUARE_SIZE - offset * getXOff(cgs.getClient()->getDirection());
    const int CTOP_Y = CENTER_Y - 0.5 * SQUARE_SIZE - offset * getYOff(cgs.getClient()->getDirection());
    const int TRAIL_ALPHA = 128;

    QFont font;
    font.setPixelSize(SQUARE_SIZE / 3);
    QFontMetrics fm(font);
    painter->setFont(font);
    updateColorMap(cgs.getPlayers());

    //printing background
    painter->fillRect(event->rect(), background);

    //define center point (player is always centered, so this will always execute with fixed points)
    QPoint CenterPt;
    CenterPt.setX(CENTER_X);
    CenterPt.setY(CENTER_Y);

    //rendering square by square
    for (int x = -(CLIENT_FRAME / 2); x <= (CLIENT_FRAME / 2); ++x)
    {
        for (int y = -(CLIENT_FRAME / 2); y <= (CLIENT_FRAME / 2); ++y)
        {
            ClientSquareState state = cgs.getState(x, y);
            if (state.getOwningPlayerId() == OUT_OF_BOUNDS)
            {
                painter->fillRect(CTOP_X + x * SQUARE_SIZE,
                                  CTOP_Y + y * SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  outOfBoundsColor);
                continue;
            }
            if(state.isOwned())
            {
                painter->fillRect(CTOP_X + x * SQUARE_SIZE,
                                  CTOP_Y + y * SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  playerColors[colorMap.value(state.getOwningPlayerId())][1]);
            }

            if(state.hasTrail())
            {
                QColor trailColor = playerColors[colorMap.value(state.getTrailPlayerId())][0];
                trailColor.setAlpha(TRAIL_ALPHA);

                QPainterPath triangle;
                int triX = CTOP_X + x * SQUARE_SIZE;
                int triY = CTOP_Y + y * SQUARE_SIZE;

                switch (state.getTrailType())
                {
                case EASTTOWEST:
                case NORTHTOSOUTH:
                    painter->fillRect(CTOP_X + x * SQUARE_SIZE,
                                    CTOP_Y + y * SQUARE_SIZE,
                                    SQUARE_SIZE,
                                    SQUARE_SIZE,
                                    trailColor.lighter(125));
                    break;

                case NORTHTOEAST:
                    triangle.moveTo(triX, triY + SQUARE_SIZE);
                    triangle.lineTo(triX + SQUARE_SIZE, triY);
                    triangle.lineTo(triX + SQUARE_SIZE, triY + SQUARE_SIZE);
                    triangle.lineTo(triX, triY + SQUARE_SIZE);
                    painter->fillPath(triangle, trailColor);
                    break;

                case NORTHTOWEST:
                    triangle.moveTo(triX, triY + SQUARE_SIZE);
                    triangle.lineTo(triX, triY);
                    triangle.lineTo(triX + SQUARE_SIZE, triY + SQUARE_SIZE);
                    triangle.lineTo(triX, triY + SQUARE_SIZE);
                    painter->fillPath(triangle, trailColor);
                    break;

                case SOUTHTOEAST:
                    triangle.moveTo(triX + SQUARE_SIZE, triY + SQUARE_SIZE);
                    triangle.lineTo(triX + SQUARE_SIZE, triY);
                    triangle.lineTo(triX, triY);
                    triangle.lineTo(triX + SQUARE_SIZE, triY + SQUARE_SIZE);
                    painter->fillPath(triangle, trailColor);
                    break;

                case SOUTHTOWEST:
                    triangle.moveTo(triX, triY + SQUARE_SIZE);
                    triangle.lineTo(triX, triY);
                    triangle.lineTo(triX + SQUARE_SIZE, triY);
                    triangle.lineTo(triX, triY + SQUARE_SIZE);
                    painter->fillPath(triangle, trailColor);
                    break;

                default:
                    break;

                }
            }
        }
    }
    for (int x = -(CLIENT_FRAME / 2); x <= (CLIENT_FRAME / 2); ++x)
    {
        for (int y = -(CLIENT_FRAME / 2); y <= (CLIENT_FRAME / 2); ++y)
        {
            ClientSquareState state = cgs.getState(x, y);

            if(state.isOccupied() && state.getOccupyingPlayer())
            {
                Direction squarePlayer = state.getOccupyingPlayer()->getDirection();
                int playerX = CTOP_X + x * SQUARE_SIZE + offset * getXOff(squarePlayer);
                int playerY = CTOP_Y + y * SQUARE_SIZE + offset * getYOff(squarePlayer);

                int textX = playerX + SQUARE_SIZE / 2 - fm.width(state.getOccupyingPlayer()->getName()) / 2;
                QColor playerColor = playerColors[colorMap.value(state.getOccupyingPlayerId())][0];

                painter->fillRect(playerX,
                                  playerY,
                                  SQUARE_SIZE,
                                  SQUARE_SIZE,
                                  playerColor);

                playerColor.setAlpha(128);
                painter->setPen(QPen(playerColor.darker(250)));
                painter->drawText(textX + 2, playerY - 9, state.getOccupyingPlayer()->getName());

                playerColor.setAlpha(255);
                painter->setPen(QPen(playerColor.darker(120)));
                painter->drawText(textX, playerY - 10, state.getOccupyingPlayer()->getName());
            }
        }
    }
}


const uint8_t ARDUINO_COLORS[10][3] = { {207, 175,  90},   // Magenta
                                        {196, 174,  52},   // Red
                                        { 46, 120,  22},   // Green
                                        { 21,  68,  17},   // Blue
                                        {214, 173,  94},   // Orange
                                        { 99, 141,  54},   // Purple
                                        {149, 107,  64},   // Eucalyptus
                                        { 87, 116,  23},   // Cyan
                                        { 11, 186, 100},   // Yellow
                                        {217, 138, 131} }; // Salmon

const uint16_t ARDUINO_OOB = 242;

void renderGameArduino(const ClientGameState &cgs, BufferGFX &gfx)
{
	updateColorMap(cgs.getPlayers());

	gfx.fillScreen(0);

	// The offset is -1 for the first half of the tick and 0 for the second half.
	int offset = (3 * cgs.getLastTick().msecsTo(QDateTime::currentDateTime())) / cgs.getTickRate();
	offset = std::min(offset - 1, 0);

	const ClientPlayer *client = cgs.getClient();
	int xoff = -1 * offset * getXOff(client->getDirection());
	int yoff = -1 * offset * getYOff(client->getDirection());

	for (int x = -9; x <= 9; ++x)
	{
		for (int y = -5; y<= 5; ++y)
		{
			ClientSquareState ss = cgs.getState(x, y);
			if (ss.getOwningPlayerId() == OUT_OF_BOUNDS)
			{
				gfx.fillRect(x * 2 + 15 + xoff, y * 2 + 7 + yoff, 2, 2, ARDUINO_OOB);
				continue;
			}
             
			if(ss.isOwned())
			{
				uint16_t cl = ARDUINO_COLORS[colorMap.value(ss.getOwningPlayerId())][2];
				gfx.fillRect(x * 2 + 15 + xoff, y * 2 + 7 + yoff, 2, 2, cl);
            }

			if(ss.hasTrail())
			{
				uint16_t cl = ARDUINO_COLORS[colorMap.value(ss.getTrailPlayerId())][1];
				gfx.fillRect(x * 2 + 15 + xoff, y * 2 + 7 + yoff, 2, 2, cl);
			}
		}
	}

	// We have two loops so that the players show on top.
	for (int x = -9; x <= 9; ++x)
	{
		for (int y = -5; y<= 5; ++y)
		{
			ClientSquareState ss = cgs.getState(x, y);
			if (ss.isOccupied() && ss.getOccupyingPlayer())
			{
				const ClientPlayer *lpl = ss.getOccupyingPlayer();
				int lxo = offset * getXOff(lpl->getDirection());
				int lyo = offset * getYOff(lpl->getDirection());
				uint16_t cl = ARDUINO_COLORS[colorMap.value(lpl->getId())][0];
				gfx.fillRect(x * 2 + 15 + xoff + lxo, y * 2 + 7 + yoff + lyo, 2, 2, cl);
			}
		}
	}
}
