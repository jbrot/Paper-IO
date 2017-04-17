/*
 * This provides the implementation of BufferGFX.
 */

#include <algorithm>

#include "buffergfx.h"

BufferGFX::BufferGFX()
	: Adafruit_GFX(ARDUINO_WIDTH, ARDUINO_HEIGHT)
{
	std::fill(&buffer[0][0], &buffer[0][0] + ARDUINO_WIDTH * ARDUINO_HEIGHT, 0);
}

uint8_t (*BufferGFX::getBuffer())[ARDUINO_WIDTH]
{
	return buffer;
}

void BufferGFX::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if (x < 0 || y < 0 || x >= ARDUINO_WIDTH || y >= ARDUINO_HEIGHT)
		return;

	buffer[y][x] = deriveColor(color);
}

void BufferGFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	if (y < 0 || y >= ARDUINO_HEIGHT || x >= ARDUINO_WIDTH)
		return;

	if (x < 0)
	{
		w += x;
		x = 0;
	}

	if (w <= 0)
		return;

	if (x + w > ARDUINO_WIDTH)
		w = ARDUINO_WIDTH - x;

	std::fill(&buffer[y][x], &buffer[y][x] + w, deriveColor(color));
}

void BufferGFX::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	if (x < 0 || x >= ARDUINO_WIDTH || y >= ARDUINO_HEIGHT)
		return;

	if (y < 0)
	{
		h += y;
		y = 0;
	}

	if (y + h > ARDUINO_HEIGHT)
		h = ARDUINO_HEIGHT - y;

	uint8_t c = deriveColor(color);
	for (int i = y; i < y + h; ++i)
		buffer[y][x] = c;
}

void BufferGFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	if (x >= ARDUINO_WIDTH || y >= ARDUINO_HEIGHT)
		return;

	if (x < 0)
	{
		w += x;
		x = 0;
	}

	if (w <= 0)
		return;

	if (x + w > ARDUINO_WIDTH)
		w = ARDUINO_WIDTH - x;

	if (y < 0)
	{
		h += y;
		y = 0;
	}

	if (h <= 0)
		return;

	if (y + h > ARDUINO_HEIGHT)
		h = ARDUINO_HEIGHT - y;

	uint8_t c = deriveColor(color);
	for (int i = y; i < y + h; i++)
		std::fill(&buffer[i][x], &buffer[i][x] + w, c);
}

void BufferGFX::fillScreen(uint16_t color)
{
	std::fill(&buffer[0][0], &buffer[0][0] + ARDUINO_WIDTH * ARDUINO_HEIGHT, deriveColor(color));
}

// This does nothing as we switched to a color table and
// it's easier to just keep track of indices to the table
// then to try and convert from RGB.
uint8_t BufferGFX::deriveColor(uint16_t color)
{
	return color & 0xFF;
}
