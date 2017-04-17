# Main Config
TEMPLATE = app
TARGET = server
CONFIG += c++11 debug

# Build/Install Directories
MOC_DIR = $$PWD/../build/server/moc
OBJECTS_DIR = $$PWD/../build/server/obj
RCC_DIR = $$PWD/../build/server/rcc
DESTDIR = $$PWD/../bin

# Meta Inputs
QT += widgets network
RESOURCES = server.qrc

CXXFLAGS += -g

# Input
INCLUDEPATH += . $$PWD/../common
HEADERS += aiplayer.h \
	clienthandler.h \
	gamehandler.h \
	gamelogic.h \
	gamestate.h \
	nicks.h \
	paperserver.h \
	../common/protocol.h \
	../common/types.h
SOURCES += main.cpp \
	aiplayer.cpp \
	clienthandler.cpp \
	gamehandler.cpp \
	gamelogic.cpp \
	gamestate.cpp \
	nicks.cpp \
	paperserver.cpp \
	player.cpp \
	squarestate.cpp \
# Common files
	../common/packetgameend.cpp \
	../common/packetgamejoin.cpp \
	../common/packetgametick.cpp \
	../common/packetleaderboardupdate.cpp \
	../common/packetplayersupdate.cpp \
	../common/packetrequestjoin.cpp \
	../common/packetresendboard.cpp \
	../common/packetupdatedir.cpp \
	../common/protocol.cpp

