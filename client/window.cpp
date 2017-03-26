/*
 * This is the implementation of the client window. It
 * sets up the GUI and begins the setup of the other
 * aspects when appropriate.
 */

#include <QFormLayout>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtCore/QStringBuilder>
#include <QVBoxLayout>

#include "window.h"

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

Window::Window()
{
	setWindowTitle(tr("Arduino-IO"));

	// Load custom font. Code from http://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
	int id = QFontDatabase::addApplicationFont(":/fonts/Freshman.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont freshman(family);
	freshman.setPointSize(72);
	
	QLabel *title = new QLabel(getTitleString(tr("Arduino-IO")));
	title->setTextInteractionFlags(Qt::NoTextInteraction);
	title->setAlignment(Qt::AlignCenter);
	title->setFont(freshman);
	title->setStyleSheet("QLabel { background-color: #333; }");
	title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	QLabel *name = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit();

	QLabel *ip = new QLabel(tr("Server IP Address:"));
	QLineEdit *ipEdit = new QLineEdit();
	QLabel *port = new QLabel(tr("Server Port:"));
	QLineEdit *portEdit = new QLineEdit();

	QPushButton *connect = new QPushButton(tr("Connect"));
	connect->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QFormLayout *flayout = new QFormLayout();
	flayout->addRow(name, nameEdit);
	flayout->addRow(ip, ipEdit);
	flayout->addRow(port, portEdit);
	flayout->setSizeConstraint(QLayout::SetFixedSize);

	QSpacerItem *ls = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem *rs = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->addSpacerItem(ls);
	hlayout->addLayout(flayout);
	hlayout->addWidget(connect);
	hlayout->addSpacerItem(rs);
	hlayout->setSpacing(10);

	QVBoxLayout *vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->addWidget(title);
	vlayout->addLayout(hlayout);

	setLayout(vlayout);
}

QSize Window::sizeHint() const {
	return QSize(640, 480);
}
