/*
 * This is the implementaton of Launcher, the initial client GUI.
 */

#include <QHBoxLayout>
#include <QGridLayout>
#include <QIntValidator>
#include <QSettings>
#include <QtCore/QStringBuilder>

#include "font.h"
#include "launcher.h"

// TODO Merge with copy in gameover.cpp
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

Launcher::Launcher(QWidget *parent)
	: QWidget(parent)
	, status(new QLabel())
	, nameEdit(new QLineEdit())
	, ipEdit(new QLineEdit())
	, portEdit(new QLineEdit())
	, ctc(new QPushButton(tr("CONNECT")))
	, enabled(true)
	, ctenabled(true)
{
	QFont freshman = getFreshmanFont();
	freshman.setPointSize(72);
	
	// GUI Set up
	QLabel *title = new QLabel(getTitleString(tr("Arduino-IO")));
	title->setTextInteractionFlags(Qt::NoTextInteraction);
	title->setAlignment(Qt::AlignCenter);
	title->setFont(freshman);

	status->setAlignment(Qt::AlignLeft);
	status->setStyleSheet("QLabel { background-color: #333; color: #FFF; }");
	status->setIndent(10);
	status->setMargin(3);

	QSettings settings;
	settings.beginGroup(QLatin1String("launcher"));

	nameEdit->setText(settings.value(QLatin1String("name"), QLatin1String("")).toString());
	nameEdit->setPlaceholderText("Your Name");
	nameEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
	nameEdit->setMaximumWidth(400);

	ipEdit->setText(settings.value(QLatin1String("ip"), QLatin1String("")).toString());
	ipEdit->setPlaceholderText("Server IP");
	ipEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
	ipEdit->setMaximumWidth(350);

	portEdit->setText(settings.value(QLatin1String("port"), QLatin1String("")).toString());
	portEdit->setPlaceholderText("Port");
	portEdit->setValidator(new QIntValidator(1, 65535, this));
	portEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
	portEdit->setMaximumWidth(150);
	portEdit->setMaxLength(5);

	settings.endGroup();

	ctc->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	// If we have saved settings, then we should allow the user to connect immediately
	toggleConnect();

	QHBoxLayout *nlayout = new QHBoxLayout();
	nlayout->setContentsMargins(0, 0, 0, 0);
	nlayout->setSpacing(0);
	nlayout->addItem(new QSpacerItem(10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));
	nlayout->addWidget(nameEdit);
	nlayout->addItem(new QSpacerItem(10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));

	QHBoxLayout *slayout = new QHBoxLayout();
	slayout->setContentsMargins(0, 0, 0, 0);
	slayout->setSpacing(0);
	slayout->addItem(new QSpacerItem(10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));
	slayout->addWidget(ipEdit);
	slayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
	slayout->addWidget(portEdit);
	slayout->addItem(new QSpacerItem(10, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));

	QGridLayout *layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 0, 0, 1, 5);
	layout->addItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding), 1, 0, 7, 1);
	layout->addItem(new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding), 1, 4, 7, 1);
	layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 8, 0, 1, 5);

	layout->addWidget(title, 1, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 2, 1, 1, 3);
	layout->addLayout(nlayout, 3, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 4, 1, 1, 3);
	layout->addLayout(slayout, 5, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed), 6, 1, 1, 3);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 7, 1, 1, 1);
	layout->addWidget(ctc, 7, 2);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding), 7, 3, 1, 1);

	layout->addWidget(status, 9, 0, 1, 5);

	setLayout(layout);

	connect(nameEdit, &QLineEdit::textChanged, this, &Launcher::toggleConnect);
	connect(ipEdit, &QLineEdit::textChanged, this, &Launcher::toggleConnect);
	connect(portEdit, &QLineEdit::textChanged, this, &Launcher::toggleConnect);
	connect(ctc, &QAbstractButton::clicked, this, &Launcher::doConnect);
}

void Launcher::toggleConnect()
{
	ctc->setEnabled(enabled && ctenabled && !nameEdit->text().isEmpty()
	                                     && !ipEdit->text().isEmpty()
	                                     && !portEdit->text().isEmpty());
}

void Launcher::enable()
{
	enabled = true;
	nameEdit->setEnabled(true);
	ipEdit->setEnabled(true);
	portEdit->setEnabled(true);
	toggleConnect();
}

void Launcher::disable()
{
	enabled = false;
	nameEdit->setEnabled(false);
	ipEdit->setEnabled(false);
	portEdit->setEnabled(false);
	toggleConnect();
}

void Launcher::enableConnect()
{
	ctenabled = true;
	toggleConnect();
}

void Launcher::disableConnect()
{
	ctenabled = false;
	toggleConnect();
}

void Launcher::doConnect()
{
	QSettings settings;
	settings.beginGroup(QLatin1String("launcher"));
	settings.setValue(QLatin1String("name"), nameEdit->text());
	settings.setValue(QLatin1String("ip"), ipEdit->text());
	settings.setValue(QLatin1String("port"), portEdit->text());
	settings.endGroup();

	emit connectToServer(ipEdit->text(), portEdit->text().toInt(), nameEdit->text());
}

void Launcher::setStatus(const QString &msg)
{
	status->setText(msg);
}
