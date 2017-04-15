/*
 * Loads and processes the nicks file.
 */

#include <algorithm>
#include <QFile>
#include <QtCore>
#include <QTextStream>
#include <QVector>

#include "nicks.h"

QString getRandomNick()
{
	static QVector<QString> names;
	static int index = 0;
	if (names.empty())
	{
		QFile nicks(":/res/nicks.txt");
		if (!nicks.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qWarning() << "Could not load nick names. Defaulting to \"AI\".";
			names << QLatin1String("AI");
			return names[0];
		}

		names.reserve(1000);
		QTextStream in(&nicks);
		while (!in.atEnd())
			names << in.readLine();

		nicks.close();

		// So we don't get the same nicks each time
		std::random_shuffle(names.begin(), names.end());
	}

	// By incrementing first, we only have to do one out of bounds check.
	index = (index + 1) % names.size();
	return names[index];
}

