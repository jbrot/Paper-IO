/*
 * This widget draws the game.
 */

#include <QMessageBox>
#include <QPainter>
#include <QtCore>

#include "gamewidget.h"
#include "render.h"

const int KSTR[] = { Qt::Key_E, Qt::Key_T, Qt::Key_A, Qt::Key_O, 
                     Qt::Key_I, Qt::Key_N, Qt::Key_S, Qt::Key_H,
                     Qt::Key_R, Qt::Key_D, Qt::Key_L, Qt::Key_U };

GameWidget::GameWidget(ClientGameState &gs, QWidget *parent)
	: QOpenGLWidget(parent)
	, cgs(gs)
	, kiosk(false)
	, ks(0)
{
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_MacShowFocusRect, 0);
}

bool GameWidget::isKiosk()
{
	return kiosk;
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
	if (event->key() == KSTR[ks])
	{
		ks++;
		qWarning() << "ADVANCE" << ks;
		if (ks == 12)
		{
			kiosk = !kiosk;
			ks = 0;
			emit changeKiosk(kiosk);
		}
	} else {
		ks = 0;
	}

	if (!kiosk)
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
	}

	QOpenGLWidget::keyPressEvent(event);
}
