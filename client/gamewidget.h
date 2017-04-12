/*
 * This widget is responsible for drawing the game, although
 * it is basically jsuta  front end for render.h
 */

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QOpenGLWidget>

#include "clientgamestate.h"

class GameWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	GameWidget(ClientGameState &cgs, QWidget *parent = Q_NULLPTR);

signals:
	void changeDirection(Direction dir);

public slots:
	void animate();

protected:
	void paintEvent(QPaintEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;

private:
	ClientGameState &cgs;
};

#endif // !GAMEWIDGET_H
