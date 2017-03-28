# Main Config
TEMPLATE = app
TARGET = client

# Build/Install Directories
MOC_DIR = $$PWD/../build/client/moc
OBJECTS_DIR = $$PWD/../build/client/obj
RCC_DIR = $$PWD/../build/client/rcc
DESTDIR = $$PWD/../bin

# Meta Inputs
QT += widgets network
RESOURCES = client.qrc

# Inputs
INCLUDEPATH += . $$PWD/../common
HEADERS += client.h \
	iohandler.h \
	launcher.h \
	protocol.h \
	types.h
SOURCES += main.cpp client.cpp iohandler.cpp launcher.cpp

