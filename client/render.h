/*
 * Provides an interface for the render code.
 */

#ifndef RENDER_H
#define RENDER_H

#include <QPainter>
#include <QPaintEvent>

#include "clientgamestate.h"

void renderGame(ClientGameState &cgs, QPainter *painter, QPaintEvent *event);

#endif // !RENDER_H
