/*
 * Type definitions for cross-platform standardization and
 * general-purpose enums.
 */
#ifndef PI_TYPES_H
#define PI_TYPES_H

typedef uint8_t plid_t;
typedef uint16_t pos_t;
typedef uint32_t state_t;

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
