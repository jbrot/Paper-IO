/*
 * Implements the waiting screen.
 */

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QSpacerItem>

#include "font.h"
#include "waiting.h"

Waiting::Waiting(QWidget *parent)
	: QWidget(parent)
{
	QFont font;
	font.setBold(true);
	font.setPointSize(64);
	font.setCapitalization(QFont::SmallCaps);

	QLabel *text = new QLabel(tr("Waiting for Game..."));
	text->setTextInteractionFlags(Qt::NoTextInteraction);
	text->setAlignment(Qt::AlignCenter);
	text->setFont(font);
	text->setStyleSheet("QLabel { background-color: #333; color: #FFF; }");
	text->setFrameStyle(QFrame::Plain | QFrame::NoFrame);

	QPushButton *cancel = new QPushButton(tr("CANCEL"));

	QGridLayout *layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 0, 0, 1, 5);
	layout->addItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding), 1, 0, 3, 1);
	layout->addItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding), 1, 4, 3, 1);
	layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 4, 0, 1, 5);

	layout->addWidget(text, 1, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 20), 2, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 3, 1, 1, 1);
	layout->addWidget(cancel, 3, 2);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 3, 3, 1, 1);

	setLayout(layout);

	connect(cancel, &QAbstractButton::clicked, this, &Waiting::cancel);
}
