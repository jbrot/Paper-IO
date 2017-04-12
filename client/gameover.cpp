/*
 * Implements the game over screen.
 */

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QSpacerItem>
#include <QtCore>
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
	QLabel *title = new QLabel(getTitleString(tr("Arduino-IO")));
	title->setTextInteractionFlags(Qt::NoTextInteraction);
	title->setAlignment(Qt::AlignCenter);
	title->setFont(getFreshmanFont());
	title->setStyleSheet("QLabel { font-size: 72px; }");

	msg->setTextInteractionFlags(Qt::NoTextInteraction);
	msg->setAlignment(Qt::AlignCenter);
	msg->setStyleSheet("QLabel { font-size: 54px; font-weight: 600; font-variant: small-caps; }");

	QGridLayout *layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setHorizontalSpacing(0);
	layout->setVerticalSpacing(20);

	// Outside spacing
	layout->setRowMinimumHeight(0, 20);
	layout->setRowStretch(0, 1);
	layout->setRowMinimumHeight(5, 20);
	layout->setRowStretch(5, 1);
	layout->setColumnMinimumWidth(0, 20);
	layout->setColumnStretch(0, 1);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnStretch(4, 1);

	// Center
	layout->addWidget(title, 1, 1, 1, 3);
	layout->addWidget(msg, 2, 1, 1, 3);
	layout->addWidget(again, 3, 2);
	layout->addWidget(quit, 4, 2);

	setLayout(layout);

	connect(again, &QAbstractButton::clicked, this, &GameOver::playAgain);
	connect(again, &QAbstractButton::clicked, this, [this] {
		this->again->setEnabled(false);
	});
	connect(quit, &QAbstractButton::clicked, this, &GameOver::disconnect);
	connect(quit, &QAbstractButton::clicked, this, [this] {
		this->quit->setEnabled(false);
	});
}

void GameOver::setScore(score_t score, quint16 total)
{
	double pct = score == total ? 100 : 100 * score / (double) total;
	qDebug() << "Game Over! Score:" << score << "/" << total << ":" << pct << "%";
	msg->setText(tr("Score: %1%").arg(QString::number(pct, 'f', (score >= 10 ? 1 : 2)), 4));
	again->setEnabled(true);
	quit->setEnabled(true);
}
