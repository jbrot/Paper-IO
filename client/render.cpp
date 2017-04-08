/*
 * This file includes the actual rendering code.
 */

#include <QtCore>

#include "render.h"

static QBrush background = QBrush(QColor(64, 32, 64));

void renderGame(ClientGameState &cgs, QPainter *painter, QPaintEvent *event)
{
	qDebug() << "Tick!";
	painter->fillRect(event->rect(), background);
}
