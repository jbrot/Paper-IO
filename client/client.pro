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
QT += widgets network serialport
RESOURCES = client.qrc

CXXFLAGS += -g

# Inputs
INCLUDEPATH += . $$PWD/../common
HEADERS += adafruit_gfx.h \
	arduino.h \
	buffergfx.h \
	client.h \
	clientgamestate.h \
	font.h \
	gameover.h \
	gamewidget.h \
	gfxfont.h \
	glcdfont.h \
	iohandler.h \
	kioskai.h \
	launcher.h \
	render.h \
	waiting.h \
	../common/protocol.h \
	../common/types.h
SOURCES += main.cpp \
	adafruit_gfx.cpp \
	arduino.cpp \
	buffergfx.cpp \
	client.cpp \
	clientgamestate.cpp \
	clientplayer.cpp \
	clientsquarestate.cpp \
	font.cpp \
	gameover.cpp \
	gamewidget.cpp \
	iohandler.cpp \
	kioskai.cpp \
	launcher.cpp \
	render.cpp \
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


