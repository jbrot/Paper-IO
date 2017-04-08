/*
 * Custom font code.
 */

#include <QFontDatabase>

#include "font.h"

static QString loadFile(QString file)
{
	int id = QFontDatabase::addApplicationFont(file);
	return QFontDatabase::applicationFontFamilies(id).at(0);
}

QFont getFreshmanFont()
{
	static QString family;
	if (family.isNull())
		family = loadFile(":/fonts/Freshman.ttf");
	return QFont(family);
}

QFont getDejaVuFont()
{
	static QString family;
	if (family.isNull())
	{
		// Load the main font and auxiliary, which will be selected
		// should properties be configured differently.
		family = loadFile(":/fonts/DejaVuSans.ttf");
		loadFile(":/fonts/DejaVuSans-Bold.ttf");
	}
	return QFont(family);
}
