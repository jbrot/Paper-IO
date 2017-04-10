/*
 * This widget draws the game.
 */

#include <QPainter>
#include <QtCore>

#include "gamewidget.h"
#include "render.h"

GameWidget::GameWidget(ClientGameState &gs, QWidget *parent)
	: QOpenGLWidget(parent)
	, cgs(gs)
{
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_MacShowFocusRect, 0);
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

void GameWidget::keyPressEvent(QKeyEvent *event)
{
	qDebug() << "Key press:" << event->text();

	QOpenGLWidget::keyPressEvent(event);
}
