######################################################################
# Automatically generated by qmake (3.0) Sun Mar 26 15:20:20 2017
######################################################################

TEMPLATE = app
TARGET = server
INCLUDEPATH += .

QT += widgets network

# Input
HEADERS += gamelogic.h \
	GameState.h \
	PaperServer.h \
	Player.h \
	types.h
SOURCES += main.cpp PaperServer.cpp Player.cpp \
	gamelogic.cpp
