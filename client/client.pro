# Main Config
TEMPLATE = app
TARGET = client
CONFIG += c++11 debug

# Build/Install Directories
MOC_DIR = $$PWD/../build/client/moc
OBJECTS_DIR = $$PWD/../build/client/obj
RCC_DIR = $$PWD/../build/client/rcc
DESTDIR = $$PWD/../bin

# Meta Inputs
QT += widgets network
RESOURCES = client.qrc

CXXFLAGS += -g

# Inputs
INCLUDEPATH += . $$PWD/../common
HEADERS += client.h \
	clientgamestate.h \
	font.h \
	gameover.h \
	iohandler.h \
	launcher.h \
	waiting.h \
	../common/protocol.h \
	../common/types.h
SOURCES += main.cpp \
	client.cpp \
	clientgamestate.cpp \
	clientplayer.cpp \
	clientsquarestate.cpp \
	font.cpp \
	gameover.cpp \
	iohandler.cpp \
	launcher.cpp \
	waiting.cpp \
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


