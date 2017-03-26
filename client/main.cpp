/*
 * This is the main entry point for the paper-io client. 
 */

#include <QApplication>

#include "window.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Window window;
	window.show();

	return app.exec();
}
