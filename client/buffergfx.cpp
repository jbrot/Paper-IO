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

// Possibly gamma correct or something?
uint8_t BufferGFX::deriveColor(uint16_t color)
{
	uint8_t r = (color & 0xE000) >> 8;
	uint8_t g = (color & 0x0700) >> 6;
	uint8_t b = (color & 0x0018) >> 3;

	return r | g | b;
}

// These conversion functions are copied from RGBMatrixPanel.cpp
// It is BSD Licensed.
//
// N.B. I didn't bother to set up gamma correcting. Perhaps it's worth
// looking into at some point (but probably not).

// Promote 3/3/3 RGB to Adafruit_GFX 5/6/5
uint16_t BufferGFX::Color333(uint8_t r, uint8_t g, uint8_t b) {
  // RRRrrGGGgggBBBbb
  return ((r & 0x7) << 13) | ((r & 0x6) << 10) |
         ((g & 0x7) <<  8) | ((g & 0x7) <<  5) |
         ((b & 0x7) <<  2) | ((b & 0x6) >>  1);
}

// Promote 4/4/4 RGB to Adafruit_GFX 5/6/5
uint16_t BufferGFX::Color444(uint8_t r, uint8_t g, uint8_t b) {
  // RRRRrGGGGggBBBBb
  return ((r & 0xF) << 12) | ((r & 0x8) << 8) |
         ((g & 0xF) <<  7) | ((g & 0xC) << 3) |
         ((b & 0xF) <<  1) | ((b & 0x8) >> 3);
}

// Demote 8/8/8 to Adafruit_GFX 5/6/5
// If no gamma flag passed, assume linear color
uint16_t BufferGFX::Color888(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 11) | ((g & 0xFC) << 5) | (b >> 3);
}


uint16_t BufferGFX::ColorHSV(long hue, uint8_t sat, uint8_t val) {

  uint8_t  r, g, b, lo;
  uint16_t s1, v1;

  // Hue
  hue %= 1536;             // -1535 to +1535
  if(hue < 0) hue += 1536; //     0 to +1535
  lo = hue & 255;          // Low byte  = primary/secondary color mix
  switch(hue >> 8) {       // High byte = sextant of colorwheel
    case 0 : r = 255     ; g =  lo     ; b =   0     ; break; // R to Y
    case 1 : r = 255 - lo; g = 255     ; b =   0     ; break; // Y to G
    case 2 : r =   0     ; g = 255     ; b =  lo     ; break; // G to C
    case 3 : r =   0     ; g = 255 - lo; b = 255     ; break; // C to B
    case 4 : r =  lo     ; g =   0     ; b = 255     ; break; // B to M
    default: r = 255     ; g =   0     ; b = 255 - lo; break; // M to R
  }

  // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
  // on the result rather than a costly divide, while the type upgrade to int
  // avoids repeated type conversions in both directions.
  s1 = sat + 1;
  r  = 255 - (((255 - r) * s1) >> 8);
  g  = 255 - (((255 - g) * s1) >> 8);
  b  = 255 - (((255 - b) * s1) >> 8);

  // Value (brightness) & 16-bit color reduction: similar to above, add 1
  // to allow shifts, and upgrade to int makes other conversions implicit.
  v1 = val + 1;
  r = (r * v1) >> 12; // 4-bit results
  g = (g * v1) >> 12;
  b = (b * v1) >> 12;
  return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
         (g <<  7) | ((g & 0xC) << 3) |
         (b <<  1) | ( b        >> 3);
}
