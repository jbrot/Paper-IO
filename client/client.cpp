/*
 * This is the implementation of the client window. It
 * sets up the GUI and begins the setup of the other
 * aspects when appropriate.
 */

#include <QNetworkConfigurationManager>
#include <QMessageBox>
#include <QSettings>
#include <QStackedLayout>
#include <QVBoxLayout>

#include "client.h"

Client::Client(QWidget *parent)
	: QWidget(parent)
	, launcher(new Launcher)
	, cgs()
	, timeout(new QTimer(this))
	, ioh(new IOHandler(cgs))
	, iothread(new QThread(this))
	, waiting(new Waiting)
	, gameover(new GameOver)
	, session(Q_NULLPTR)
{
	setWindowTitle(tr("Arduino-IO"));

	QStackedLayout *stack = new QStackedLayout();
	stack->addWidget(launcher);
	stack->addWidget(waiting);
	QLabel *temp = new QLabel("temp", this);
	stack->addWidget(temp);
	stack->addWidget(gameover);
	//stack->setCurrentIndex(3);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(stack);
	setLayout(layout);

	// Network setup
	ioh->moveToThread(iothread);
	connect(iothread, &QThread::finished, ioh, &QObject::deleteLater);
	
	// Timeout setup
	timeout->setInterval(5000);
	timeout->setSingleShot(true);

	connect(launcher, &Launcher::connectToServer, ioh, &IOHandler::connectToServer);
	connect(launcher, &Launcher::connectToServer, timeout, static_cast<void (QTimer::*)()>(&QTimer::start));
	connect(launcher, &Launcher::connectToServer, launcher, &Launcher::disable);
	connect(launcher, &Launcher::connectToServer, this, [this] {
		this->launcher->setStatus(tr("Connecting..."));
	} );

	connect(timeout, &QTimer::timeout, this, &Client::connectTimeout);
	connect(timeout, &QTimer::timeout, ioh, &IOHandler::disconnect);
	connect(timeout, &QTimer::timeout, launcher, &Launcher::enable);

	connect(ioh, &IOHandler::error, this, &Client::displayError);
	connect(ioh, &IOHandler::error, launcher, &Launcher::enable);
	connect(ioh, &IOHandler::error, timeout, &QTimer::stop);
	connect(ioh, &IOHandler::connected, timeout, &QTimer::stop);
	connect(ioh, &IOHandler::connected, ioh, &IOHandler::enterQueue);
	connect(ioh, &IOHandler::disconnected, this, &Client::disconnected);
	connect(ioh, &IOHandler::disconnected, launcher, &Launcher::enable);
	connect(ioh, &IOHandler::disconnected, stack, [stack] {
		stack->setCurrentIndex(0);
	});
	connect(ioh, &IOHandler::queued, stack, [stack] {
		stack->setCurrentIndex(1);
	});
	connect(ioh, &IOHandler::gameEnded, gameover, &GameOver::setScore);
	connect(ioh, &IOHandler::gameEnded, stack, [stack] {
		stack->setCurrentIndex(3);
	});

	connect(waiting, &Waiting::cancel, ioh, &IOHandler::disconnect);

	connect(gameover, &GameOver::playAgain, ioh, &IOHandler::enterQueue);
	connect(gameover, &GameOver::disconnect, ioh, &IOHandler::disconnect);

	iothread->start();

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
		connect(session, &QNetworkSession::opened, launcher, &Launcher::enableConnect);
		typedef void (QNetworkSession::*QNetworkSessionErrorSignal)(QNetworkSession::SessionError);
		connect(session, static_cast<QNetworkSessionErrorSignal>(&QNetworkSession::error),
		        this, &Client::displayError2);

		launcher->setStatus(tr("Opening network session..."));
		session->open();
	} else {
		launcher->setStatus(tr("Ready."));
	}
}

Client::~Client()
{
	// Kill the IO thread
	// Adapted from http://stackoverflow.com/questions/28660852/qt-qthread-destroyed-while-thread-is-still-running-during-closing
	iothread->quit();
	if (!iothread->wait(1000)) // Wait for termination (1 sec max)
	{
		iothread->terminate();
		iothread->wait();
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

	launcher->setStatus(tr("Ready."));
}

void Client::displayError(QAbstractSocket::SocketError socketError, QString msg)
{
	launcher->setStatus(tr("Ready."));
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
		                            "Make sure the remote server is running, "
		                            "and check that the host name and port "
		                            "settings are correct."));
		break;
	default:
		QMessageBox::information(this, tr("Arduino-IO"),
		                         tr("The following error occurred: %1.")
		                         .arg(msg));
	}
}

void Client::displayError2(QNetworkSession::SessionError sessionError)
{
	QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("The following error occurred: %1.").arg(session->errorString()));
}


void Client::disconnected()
{
	QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("Disconnected!"));
	launcher->setStatus(tr("Ready."));
}


void Client::connectTimeout()
{
	QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("Connection timed out!"));
	launcher->setStatus(tr("Ready."));
}
