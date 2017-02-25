#ifndef _Adafruit_v2_h
#define _Adafruit_v2_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define ANZ_MESSUNGEN_PRO_SCAN 9

enum MOVEMENT {
	VORWAERTS, VOR_DIAG_RECHTS, VOR_DIAG_LINKS,
	LINKS, RECHTS,
	RUECKWAERTS, RUECK_DIAG_RECHTS, RUECK_DIAG_LINKS,
	DREHEN_LINKS, DREHEN_RECHTS, 
	MOVEMENT_COUNT
};

#endif


