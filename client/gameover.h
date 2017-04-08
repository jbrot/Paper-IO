/*
 * This class draws the Game Over screen after the player dies. It provides
 * an option to play again and disconnect.
 */

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

class GameOver : public QWidget
{
	Q_OBJECT

public:
	GameOver(QWidget *parent = Q_NULLPTR);

public slots:
	void setScore(quint8 score);

signals:
	void playAgain();
	void disconnect();

private:
	QLabel *msg;
	QPushButton *again;
	QPushButton *quit;
};

#endif // !GAMEOVER_H
