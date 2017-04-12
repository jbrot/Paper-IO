/*
 * This is the implementation of the client window. It
 * sets up the GUI and begins the setup of the other
 * aspects when appropriate.
 */

#include <QNetworkConfigurationManager>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

#include "client.h"

Client::Client(QWidget *parent)
	: QWidget(parent)
	, cgs()
	, stack(new QStackedLayout)
	, session(Q_NULLPTR)
	, timeout(new QTimer(this))
	, ioh(new IOHandler(cgs))
	, iothread(new QThread(this))
	, disconnecting(false)
	, arduino(new Arduino(cgs, this))
	, launcher(new Launcher)
	, waiting(new Waiting)
	, render(new GameWidget(cgs))
	, rtimer(new QTimer(this))
	, gameover(new GameOver)
{
	setWindowTitle(tr("Arduino-IO"));

	stack->addWidget(launcher);
	stack->addWidget(waiting);
	stack->addWidget(render);
	stack->addWidget(gameover);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(stack);
	setLayout(layout);

	rtimer->setInterval(30);

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
	connect(launcher, &Launcher::connectToArduino, this, &Client::connectToArduino);

	connect(timeout, &QTimer::timeout, this, &Client::connectTimeout);
	connect(timeout, &QTimer::timeout, ioh, &IOHandler::disconnect);
	connect(timeout, &QTimer::timeout, launcher, &Launcher::enable);

	connect(ioh, &IOHandler::error, this, &Client::displayError);
	connect(ioh, &IOHandler::error, launcher, &Launcher::enable);
	connect(ioh, &IOHandler::error, timeout, &QTimer::stop);

	connect(ioh, &IOHandler::connected, timeout, &QTimer::stop);
	connect(ioh, &IOHandler::connected, ioh, &IOHandler::enterQueue);

	connect(ioh, &IOHandler::disconnected, this, [this] {
		disconnecting = false;
		this->launcher->setStatus(tr("Ready."));
	});
	connect(ioh, &IOHandler::disconnected, launcher, &Launcher::enable);
	connect(ioh, &IOHandler::disconnected, rtimer, &QTimer::stop);
	connect(ioh, &IOHandler::disconnected, render, &QWidget::clearFocus);
	connect(ioh, &IOHandler::disconnected, arduino, &Arduino::renderLauncher);
	connect(ioh, &IOHandler::disconnected, stack, [this] {
		this->stack->setCurrentIndex(0);
	});

	connect(ioh, &IOHandler::queued, stack, [this] {
		this->stack->setCurrentIndex(1);
	});
	connect(ioh, &IOHandler::queued, arduino, &Arduino::renderWaiting);

	connect(ioh, &IOHandler::enteredGame, rtimer, static_cast<void (QTimer::*)()>(&QTimer::start));
	connect(ioh, &IOHandler::enteredGame, stack, [this] {
		this->stack->setCurrentIndex(2);
	});
	connect(ioh, &IOHandler::enteredGame, render, static_cast<void (QWidget::*)()>(&QWidget::setFocus));
	connect(ioh, &IOHandler::enteredGame, arduino, &Arduino::renderTick);

	connect(ioh, &IOHandler::gameTick, arduino, &Arduino::renderTick);

	connect(ioh, &IOHandler::gameEnded, rtimer, &QTimer::stop);
	connect(ioh, &IOHandler::gameEnded, render, &QWidget::clearFocus);
	connect(ioh, &IOHandler::gameEnded, gameover, &GameOver::setScore);
	connect(ioh, &IOHandler::gameEnded, arduino, &Arduino::renderGameOver);
	connect(ioh, &IOHandler::gameEnded, stack, [this] {
		this->stack->setCurrentIndex(3);
	});

	connect(waiting, &Waiting::cancel, this, [this] {
		this->disconnecting = true;
	});
	connect(waiting, &Waiting::cancel, ioh, &IOHandler::disconnect);

	connect(rtimer, &QTimer::timeout, render, &GameWidget::animate);
	connect(render, &GameWidget::changeDirection, ioh, &IOHandler::changeDirection, Qt::QueuedConnection);

	connect(gameover, &GameOver::playAgain, ioh, &IOHandler::enterQueue);
	connect(gameover, &GameOver::disconnect, this, [this] {
		this->disconnecting = true;
	});
	connect(gameover, &GameOver::disconnect, ioh, &IOHandler::disconnect);

	connect(arduino, &Arduino::disconnected, launcher, &Launcher::enableArduino);
	connect(arduino, &Arduino::errorOccurred, this, &Client::displayError3);

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
		if (disconnecting)
			QMessageBox::information(this, tr("Arduino-IO"),
			                         tr("The server closed the connection!"));
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

void Client::displayError3(QSerialPort::SerialPortError error, QString msg)
{
	if (error == QSerialPort::ResourceError)
		QMessageBox::information(this, tr("Arduino-IO"),
		                         tr("The Arduino disconnected!"));
	else
		QMessageBox::information(this, tr("Arduino-IO"),
		                         tr("The following error occurred: %1 -- %2.").arg(error).arg(msg));
}

void Client::connectTimeout()
{
	disconnecting = true;
	QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("Connection timed out!"));
	launcher->setStatus(tr("Ready."));
}

void Client::connectToArduino()
{
	launcher->disableArduino();

	int res = arduino->connectToArduino();
	// We'll treat already connected the same as  establishing a successful connection.
	// It shouldn't happen anyway if we disabvle the buttons correctly.
	if (res == 0 ||res == -1)
	{
		QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("Connected to Arduino!"));
		// TODO Come up with something more robust :/
		QTimer::singleShot(1000, arduino, &Arduino::renderLauncher);
	} else if (res == -2) {
		QMessageBox::information(this, tr("Arduino-IO"),
	                         tr("No Arduino could be found! Please make sure that the Arduino is properly connected and turned on."));
		launcher->enableArduino();
	}
	// If we have error code -3, we'll assume displayError3 will handle it for us.
}
