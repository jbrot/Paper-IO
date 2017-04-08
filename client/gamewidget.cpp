/*
 * This widget draws the game.
 */

#include "gamewidget.h"
#include "render.h"

#include <QPainter>

GameWidget::GameWidget(ClientGameState &gs, QWidget *parent)
	: QOpenGLWidget(parent)
	, cgs(gs)
{
}

void GameWidget::animate()
{
	update();
}

void GameWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	cgs.lockState();
	renderGame(cgs, &painter, event);
	cgs.unlock();
	painter.end();
}
