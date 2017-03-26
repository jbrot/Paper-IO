/*
 * This class is the main window of the Paper-IO client and
 * handles most of the high-level set up.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class Window : public QWidget
{
	Q_OBJECT

public:
	Window();

	QSize sizeHint() const override;
};

#endif // !Window_H
