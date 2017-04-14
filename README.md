# Paper-IO for Arduino

This is an implementation of the online game "Paper-IO" (found at http://paper-io.com/ ) for the PC in C++ with an optional Arduino UI.
It is made with permission.

## Building

The project is made using the Qt library. To build this project, install Qt 5.8, go to the root directory and type "qmake", and then "make".
There should now be a binary in your directory which you can either double click to run or launch from the command line.

## Licenses

The licensing behind this project is ambiguous at the moment, but it doesn't really matter as this project will not be used for widespread release and we are willing to disclose source code with any distribution.

As of right now, we are using the Qt Library for networking and graphics; it is either licensed under GPL or LGPL (to be determined).

I have just added the Freshman.ttf font for the client. It is from http://fontspace.com/william-boyd/freshman and is in the public domain.

I have added the DejaVu fonts. They are under their own free license.

I have added the Adafruit\_GFX library for rendering to the arduino buffer. It is BSD licensed. Furthermore, an excerpt of the RGBMatrixPanel library is used (also BSD Licensed).
