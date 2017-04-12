/*
 * Type definitions for cross-platform standardization and
 * general-purpose enums.
 */
#ifndef PI_TYPES_H
#define PI_TYPES_H

/* Player ID */
typedef uint8_t plid_t;
/* In game Coordinate */
typedef int16_t pos_t;
/* State of one square in game. */
typedef uint32_t state_t;
/* Tick ID (monotonically increasing within a GameThread) */
typedef uint32_t tick_t;
/* Score */
typedef uint16_t score_t;

/* This object has no associated player */
const plid_t NULL_ID = 0;
/* This square is not occupied (in some sense) by any player */
const plid_t UNOCCUPIED = 0;
/* This square is not within the bounds of the game */
const plid_t OUT_OF_BOUNDS = 255;

/* The state of a square which is out of bounds. */
const state_t OUT_OF_BOUNDS_STATE = 0xFFFFFF00;

/* This positiion in either x or y means the object is out of view. */
const pos_t OUT_OF_VIEW = 32767;

/*
 * The possible directions the player can take on.
 * Origin is defined to be the top left of the screen
 * ie, positive x is right, positive y is down.
 */
enum Direction 
{
	NONE	 = 0,
	UP  	 = 1,
	DOWN	 = 3,
	LEFT	 = 2,
	RIGHT	 = 4,
};

/*
 * The possible types of trail that can be in a square.
 */
enum TrailType 
{
	NOTRAIL = 0,
	EASTTOWEST = 1,
	NORTHTOSOUTH = 2,
	NORTHTOEAST = 3,
	NORTHTOWEST = 4,
	SOUTHTOEAST = 5,
	SOUTHTOWEST = 6,
};

#endif // !PI_TYPES_H
