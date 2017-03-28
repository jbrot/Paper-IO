# Main Config
TEMPLATE = app
TARGET = server

# Build/Install Directories
MOC_DIR = $$PWD/../build/server/moc
OBJECTS_DIR = $$PWD/../build/server/obj
DESTDIR = $$PWD/../bin

# Meta Inputs
QT += widgets network

# Input
INCLUDEPATH += . $$PWD/../common
HEADERS += clienthandler.h \
	gamelogic.h \
	GameState.h \
	paperserver.h \
	Player.h \
	protocol.h \
	types.h
SOURCES += main.cpp clienthandler.cpp paperserver.cpp \
	Player.cpp gamelogic.cpp
