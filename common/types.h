/*
 * Type definitions for cross-platform standardization and
 * general-purpose enums.
 */
#ifndef PI_TYPES_H
#define PI_TYPES_H

/* Player ID */
typedef uint8_t plid_t;
/* In game Coordinate */
typedef uint16_t pos_t;
/* State of one square in game. */
typedef uint32_t state_t;
/* Tick ID (monotonically increasing within a GameThread) */
typedef uint32_t tick_t;

/* This object has no associated player */
const plid_t NULL_ID = 0;
/* This square is not occupied (in some sense) by any player */
const plid_t UNOCCUPIED = 0;
/* This square is not within the bounds of the game */
const plid_t OUT_OF_BOUNDS = 255;

/*
 * The possible directions the player can take on.
 * Origin is defined to be the top left of the screen
 * ie, positive x is right, positive y is down.
 */
enum Direction 
{
	NONE	 = 0,
	UP  	 = 1,
	DOWN	 = 2,
	LEFT	 = 3,
	RIGHT	 = 4,
};

#endif // !PI_TYPES_H
