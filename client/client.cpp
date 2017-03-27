/*
 * This is the implementation of the client window. It
 * sets up the GUI and begins the setup of the other
 * aspects when appropriate.
 */

#include <QFormLayout>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkConfigurationManager>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QtCore/QStringBuilder>
#include <QVBoxLayout>

#include "client.h"

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

Client::Client(QWidget *parent)
	: QWidget(parent)
	, status(new QLabel())
	, nameEdit(new QLineEdit())
	, ipEdit(new QLineEdit())
	, portEdit(new QLineEdit())
	, ctc(new QPushButton(tr("Connect")))
	, socket(new QTcpSocket(this))
	, session(Q_NULLPTR)
{
	setWindowTitle(tr("Arduino-IO"));

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

	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->addSpacerItem(ls);
	hlayout->addLayout(flayout);
	hlayout->addWidget(ctc);
	hlayout->addSpacerItem(rs);
	hlayout->setSpacing(10);

	QVBoxLayout *vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 0, 0, 10);
	vlayout->addWidget(title);
	vlayout->addLayout(hlayout);
	vlayout->addWidget(status);

	setLayout(vlayout);

	// Network set up
	// This code is adapted from the FortuneClient example
	str.setDevice(socket);
	str.setVersion(QDataStream::Qt_5_0);

	connect(nameEdit, &QLineEdit::textChanged, this, &Client::enableConnect);
	connect(ipEdit, &QLineEdit::textChanged, this, &Client::enableConnect);
	connect(portEdit, &QLineEdit::textChanged, this, &Client::enableConnect);
	connect(ctc, &QAbstractButton::clicked, this, &Client::connectToServer);

	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
	        this, &Client::displayError);

	QNetworkConfigurationManager manager;
	if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
	{
		QSettings settings;
		settings.beginGroup(QLatin1String("Network"));
		const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
		settings.endGroup();

		// If the saved network configuration isn't discovered, use the system default
		QNetworkConfiguration config = manager.configurationFromIdentifier(id);
		if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
			config = manager.defaultConfiguration();

		session = new QNetworkSession(config, this);
		connect(session, &QNetworkSession::opened, this, &Client::sessionOpened);
		typedef void (QNetworkSession::*QNetworkSessionErrorSignal)(QNetworkSession::SessionError);
		connect(session, static_cast<QNetworkSessionErrorSignal>(&QNetworkSession::error),
		        this, &Client::displayError2);

		ctc->setEnabled(false);
		status->setText(tr("Opening network session..."));
		session->open();
	}
}

QSize Client::sizeHint() const {
	return QSize(640, 480);
}

void Client::sessionOpened()
{
	QNetworkConfiguration config = session->configuration();
	QString id;
	if (config.type() == QNetworkConfiguration::UserChoice)
		id = session->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
	else
		id = config.identifier();

	QSettings settings;
	settings.beginGroup(QLatin1String("QtNetwork"));
	settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
	settings.endGroup();

	status->setText(QLatin1String(""));

	enableConnect();
}

void Client::enableConnect()
{
	ctc->setEnabled((!session || session->isOpen()) && !nameEdit->text().isEmpty()
	                                                && !ipEdit->text().isEmpty()
	                                                && !portEdit->text().isEmpty());
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		// TODO handle remote disconnect
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information(this, tr("Arduino-IO"),
		                         tr("The host was not found. Please check the "
		                            "host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr("Arduino-IO"),
		                         tr("The connection was refused by the peer. "
		                            "Make sure the fortune server is running, "
		                            "and check that the host name and port "
		                            "settings are correct."));
		break;
	default:
		QMessageBox::information(this, tr("Arduino-IO"),
		                         tr("The following error occurred: %1.")
		                         .arg(socket->errorString()));
	}
	
	ctc->setEnabled(true);
}

void Client::displayError2(QNetworkSession::SessionError sessionError)
{
	QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("The following error occurred: %1.").arg(session->errorString()));
}

void Client::connectToServer()
{
	ctc->setEnabled(false);
	socket->abort();
	socket->connectToHost(ipEdit->text(), portEdit->text().toInt());
}
