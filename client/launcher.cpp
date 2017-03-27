/*
 * This is the implementaton of Launcher, the initial client GUI.
 */

#include <QFontDatabase>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QtCore/QStringBuilder>
#include <QVBoxLayout>

#include "launcher.h"

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
	, ctc(new QPushButton(tr("Connect")))
	, enabled(true)
	, ctenabled(true)
{
	// Load custom font. Code from http://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
	int id = QFontDatabase::addApplicationFont(":/fonts/Freshman.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont freshman(family);
	freshman.setPointSize(72);
	
	// GUI Set up
	QLabel *title = new QLabel(getTitleString(tr("Arduino-IO")));
	title->setTextInteractionFlags(Qt::NoTextInteraction);
	title->setAlignment(Qt::AlignCenter);
	title->setFont(freshman);
	title->setStyleSheet("QLabel { background-color: #333; }");
	title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	status->setAlignment(Qt::AlignLeft);
	status->setStyleSheet("QLabel { background-color: #333; color: #FFF; }");
	status->setIndent(10);
	status->setMargin(3);

	QLabel *name = new QLabel(tr("Name:"));

	QLabel *ip = new QLabel(tr("Server IP Address:"));
	QLabel *port = new QLabel(tr("Server Port:"));
	portEdit->setValidator(new QIntValidator(1, 65535, this));

	ctc->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	ctc->setEnabled(false);

	QFormLayout *flayout = new QFormLayout();
	flayout->addRow(name, nameEdit);
	flayout->addRow(ip, ipEdit);
	flayout->addRow(port, portEdit);
	flayout->setSizeConstraint(QLayout::SetFixedSize);

	QSpacerItem *ls = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem *rs = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

	QSpacerItem *ms = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Fixed);

	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->addSpacerItem(ls);
	hlayout->addLayout(flayout);
	hlayout->addWidget(ctc);
	hlayout->addSpacerItem(rs);
	hlayout->setSpacing(10);

	QVBoxLayout *vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 0, 0, 10);
	vlayout->setSpacing(0);
	vlayout->addWidget(title);
	vlayout->addWidget(status);
	vlayout->addSpacerItem(ms);
	vlayout->addLayout(hlayout);

	setLayout(vlayout);

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
	emit connectToServer(ipEdit->text(), portEdit->text().toInt());
}

void Launcher::setStatus(const QString &msg)
{
	status->setText(msg);
}
