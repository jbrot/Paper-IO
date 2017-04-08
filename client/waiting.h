/*
 * This class draws the Waiting screen which is displayed after
 * requesting a game but before the game starts.
 */

#ifndef WAITING_H
#define WAITING_H

#include <QWidget>

class Waiting : public QWidget
{
	Q_OBJECT

public:
	Waiting(QWidget *parent = Q_NULLPTR);

signals:
	void cancel();

};

#endif // !WAITING_H
