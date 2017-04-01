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
	gamehandler.h \
	gamestate.h \
	paperserver.h \
	../common/protocol.h \
	../common/types.h
SOURCES += main.cpp clienthandler.cpp gamestate.cpp paperserver.cpp \
	player.cpp squarestate.cpp \
	gamelogic.cpp \
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

