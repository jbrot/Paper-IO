/*
 * Implements the game over screen.
 */

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QSpacerItem>
#include <QtCore/QStringBuilder>

#include "font.h"
#include "gameover.h"

// TODO Merge with copy in launcher.cpp
static QString getTitleString(QString base)
{
	QStringList colors;
	colors << "#F00" << "#0F0" << "#55D";
	auto citer = colors.cbegin();

	QString res;
	res.reserve(27 * base.size());
	for (auto iter = base.cbegin(); iter < base.cend(); iter++, citer++) {
		if (citer == colors.cend())
			citer = colors.cbegin();
		res += "<font color=\"" % *citer % "\">" % *iter % "</font>";
	}
	return res;
}

GameOver::GameOver(QWidget *parent)
	: QWidget(parent)
	, msg(new QLabel)
	, again(new QPushButton(tr("PLAY AGAIN")))
	, quit(new QPushButton(tr("DISCONNECT")))
{
	QFont freshman = getFreshmanFont();
	freshman.setPointSize(72);

	QFont font;
	font.setBold(true);
	font.setPointSize(54);
	font.setCapitalization(QFont::SmallCaps);
	
	QLabel *title = new QLabel(getTitleString(tr("Arduino-IO")));
	title->setTextInteractionFlags(Qt::NoTextInteraction);
	title->setAlignment(Qt::AlignCenter);
	title->setFont(freshman);

	msg->setTextInteractionFlags(Qt::NoTextInteraction);
	msg->setAlignment(Qt::AlignCenter);
	msg->setFont(font);
	msg->setStyleSheet("QLabel { background-color: #333; color: #FFF; }");
	msg->setFrameStyle(QFrame::Plain | QFrame::NoFrame);

	QPushButton *again = new QPushButton(tr("PLAY AGAIN"));
	QPushButton *quit= new QPushButton(tr("DISCONNECT"));

	QGridLayout *layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 0, 0, 1, 5);
	layout->addItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding), 1, 0, 7, 1);
	layout->addItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding), 1, 4, 7, 1);
	layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 8, 0, 1, 5);

	layout->addWidget(title, 1, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 2, 1, 1, 3);
	layout->addWidget(msg, 3, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 4, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 5, 1, 1, 1);
	layout->addWidget(again, 5, 2);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 5, 3, 1, 1);
	layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 6, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 7, 1, 1, 1);
	layout->addWidget(quit, 7, 2);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 7, 3, 1, 1);

	setLayout(layout);

	connect(again, &QAbstractButton::clicked, this, &GameOver::playAgain);
	connect(again, &QAbstractButton::clicked, this, [again] {
		again->setEnabled(false);
	});
	connect(quit, &QAbstractButton::clicked, this, &GameOver::disconnect);
	connect(quit, &QAbstractButton::clicked, this, [quit] {
		quit->setEnabled(false);
	});
}

void GameOver::setScore(quint8 score)
{
	msg->setText(tr("Score: %1%").arg(QString::number(score / 2.0, 'f', 1), 4));
	again->setEnabled(true);
	quit->setEnabled(true);
}
