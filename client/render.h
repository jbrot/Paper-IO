/*
 * Provides an interface for the render code.
 */

#ifndef RENDER_H
#define RENDER_H

#include <QPainter>
#include <QPaintEvent>

#include "buffergfx.h"
#include "clientgamestate.h"

void renderGame(const ClientGameState &cgs, QPainter *painter, QPaintEvent *event);

void renderGameArduino(const ClientGameState &cgs, BufferGFX &gfx);

#endif // !RENDER_H
