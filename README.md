# Arduino-IO—Micro Arcade Reach Project

__Team Name: Delta Epsilon__

__Team Members: Joshua Brot, Chandler Meyers, Vignesh Jagathese, Connor Puritz__

## Overview

For our reach project, we have fully implemented the online game "Paper-IO" (found at http://paper-io.com/).
We have permission from the creators of Paper-IO to make our game. Our implementation uses a variety of free resources within the permissions of their licenses. See the [Licenses](#licenses) section for details.

Our project right now has three components: a client program which runs on a computer, a server program which runs on a computer, and a sketch which runs on the arduino.
The computer programs make heavy use of the [Qt](https://www.qt.io/) API, which allows the computer programs to be platform agnostic.
We have tested on both Windows and OS X with Qt 5.7 and Qt 5.8. We have not tested on Linux, but if Qt is properly configured, it will most likely work.

## Building

The building process of Arduino-IO is automated via [qmake](http://doc.qt.io/qt-5/qmake-manual.html). This section will go over how to set up the various steps needed to compile the project.

### 1. Install Qt 5.8 Open Source

To compile our project, you need to ahve Qt installed. The Qt 5.8 Open Source installer can be found at https://www.qt.io/download-open-source/.
Other versions may work, but this is the installer we recommend.
Download the installe appropriate for your computer and use it to install the API. Note that the installer by default installs a lot of extra components which you don't necessarily need; use your best judgement as I have not completed the installation enough to know exactly which components are needed.

### 2a. Build from the Command Line

The simplest way to compile the project is to build it from the command line. Navigate to the root directory of the project and run `qmake`.
This will generate a makefile. You can then build the project via `make`. The completed binaries will be located in the `bin` sub-directory.

### 2b. Build from Qt Creator

The slightly more complicated way to build the project is via [Qt Creator](http://doc.qt.io/qtcreator/). When you installed Qt in step 1, you should have installed Qt Creator.
To build the project with Qt Creator, you need to build the client and the server separately. In Qt Creator, open up `client/client.pro` and `server/server.pro` for the client and server respectively.
Within each project, you need to select `Run qmake` from the build menu. Then you can click the green triangle in the left to build and run the project.

### 3. Rewire the Arduino

The Arduino needs to be wired such that the display is connected per the set up described in the [Space Invaders project](https://github.com/eecs183/eecs183-micro-arcade-W17#the-output).
The Arduino also needs to have two buttons wires with their output in pins 10 and 11. Pin 10 is the left button while Pin 11 is the right button.

### 4. Install the Arduino Component

The Arduino project is located in `arduino/main/main.ino`. The sketch will do nothing on its own, but when running on the Arduino while connected to the computer, the client can control its output and process its input.

## Running

### Server

The first part you have to run is the server. If you are running it from the command line, it will be located in `bin/server.exe` on Windows and `bin/server.app/Contents/MacOS/server` on OS X.
When you run the server, it will print out something like the following:

```
(main.cpp:70) Info: Listening at:  "127.0.0.1"  on port  64273
(main.cpp:70) Info: Listening at:  "::1"  on port  64273
(main.cpp:70) Info: Listening at:  "fe80::1%lo0"  on port  64273
(main.cpp:70) Info: Listening at:  "fe80::82f:1a0e:58e8:41e0%en0"  on port  64273
(main.cpp:70) Info: Listening at:  "35.2.61.78"  on port  64273
(main.cpp:70) Info: Listening at:  "fe80::c44a:bfff:fe0a:c0a1%awdl0"  on port  64273
(main.cpp:70) Info: Listening at:  "fe80::9e3d:70f0:786:1d37%utun0"  on port  64273
(main.cpp:70) Info: Listening at:  "fe80::ebd2:3611:66d7:b546%utun1"  on port  64273
```

This lets you know where the server is listening. `127.0.0.1` and `::1` are the loopback IPs which you will use if you are running the client on the same computer as the server. The others are local network IPs which you can use if you are running the client on a different computer.

### Client

Run the client after you start the server. Specify the IP and Port that the server outputted when it started and a username. Click connect to start playing!

### Arduino

If you have your Arduino configured per [Section 3](#3-Rewire-the-Arduino) and [Section 4](#4-Install-the-Arduino-Component) of the installation instructions, then you should be able to press the `Connect to Arduino` button on the main screen with the Arduino connected to have the client work with the Arduino.
Note that we have only tested this with our arduino, so you may need to taylor the detection parameters in [arduino.cpp](blob/master/client/arduino.cpp)

And that's it! Have fun!

## Playing the Game

The goal of the game is to control the entire board. When you leave your own territory, you leave a trail. When you make it back to your territory, everything surrounded by your trail (including the trail) becomes your territory.
If you hit a player's trail, that player gets killed. If someone hits your trail (including you!) you will die. If you go out of bounds, you die.
Use either the arrow keys or WASD to move. If you have the arduino connected, you can use the left and right buttons to turn left and right respectively.

## Licenses

The licensing behind this project is ambiguous at the moment, but it doesn't really matter as this project will not be used for widespread release and we are willing to disclose source code with any distribution.

As of right now, we are using the Qt Library for networking and graphics; it is either licensed under GPL or LGPL (to be determined).

I have just added the Freshman.ttf font for the client. It is from http://fontspace.com/william-boyd/freshman and is in the public domain.

I have added the DejaVu fonts. They are under their own free license.

I have added the Adafruit\_GFX library for rendering to the arduino buffer. It is BSD licensed. Furthermore, an excerpt of the RGBMatrixPanel library is used (also BSD Licensed).
