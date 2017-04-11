/*
 * This class provides an implementation of Adafruit_GFX which
 * simply stores the graphics in an array in memory. Our arduino
 * handler will then take this buffer and send it over to the actual
 * arduino via the serial port where it will be displayed.
 */

#ifndef BUFFERGFX_H
#define BUFFERGFX_H

#include "adafruit_gfx.h"

const int ARDUINO_WIDTH = 32;
const int ARDUINO_HEIGHT = 16;

class BufferGFX : public Adafruit_GFX
{
public:
	BufferGFX();

	void drawPixel(int16_t x, int16_t y, uint16_t color) override;
	void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
	void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
	void fillScreen(uint16_t color) override;

	// This syntax is kind of opaque. This is just returning a pointer
	// to the first element in buffer.
	uint8_t (*getBuffer())[ARDUINO_WIDTH];

	static uint16_t Color333(uint8_t r, uint8_t g, uint8_t b);
	static uint16_t Color444(uint8_t r, uint8_t g, uint8_t b);
	static uint16_t Color888(uint8_t r, uint8_t g, uint8_t b);
	static uint16_t ColorHSV(long hue, uint8_t sat, uint8_t val);

private:
	uint8_t buffer[ARDUINO_HEIGHT][ARDUINO_WIDTH];

	uint8_t deriveColor(uint16_t color);
};

#endif // !BUFFERGFX_H
