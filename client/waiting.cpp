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
	QLabel *text = new QLabel(tr("Waiting for Game..."));
	text->setTextInteractionFlags(Qt::NoTextInteraction);
	text->setAlignment(Qt::AlignCenter);
	text->setStyleSheet("QLabel { font-size: 64px; font-weight: 600; font-variant: small-caps; }");

	QPushButton *cancel = new QPushButton(tr("CANCEL"));

	QGridLayout *layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setVerticalSpacing(20);
	layout->setHorizontalSpacing(0);

	// Outside spacing
	layout->setRowMinimumHeight(0, 20);
	layout->setRowStretch(0, 1);
	layout->setRowMinimumHeight(3, 20);
	layout->setRowStretch(3, 1);
	layout->setColumnMinimumWidth(0, 20);
	layout->setColumnStretch(0, 1);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnStretch(4, 1);

	// Center
	layout->addWidget(text, 1, 1, 1, 3);
	layout->addWidget(cancel, 2, 2);

	setLayout(layout);

	connect(cancel, &QAbstractButton::clicked, this, &Waiting::cancel);
}
