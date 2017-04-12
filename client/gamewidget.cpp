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
	if (event->key() == Qt::Key_Up)
		emit changeDirection(UP);
	else if (event->key() == Qt::Key_Left)
		emit changeDirection(LEFT);
	else if (event->key() == Qt::Key_Down)
		emit changeDirection(DOWN);
	else if (event->key() == Qt::Key_Right)
		emit changeDirection(RIGHT);
	else if (event->key() == Qt::Key_W)
		emit changeDirection(UP);
	else if (event->key() == Qt::Key_A)
		emit changeDirection(LEFT);
	else if (event->key() == Qt::Key_S)
		emit changeDirection(DOWN);
	else if (event->key() == Qt::Key_D)
		emit changeDirection(RIGHT);

	QOpenGLWidget::keyPressEvent(event);
}
