// Distance.h

#ifndef _DISTANCE_h
#define _DISTANCE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif

#include "Adafruit_v2.h"

//
// Die Struktur der Parameter des Infrarot-Entfernungsmessers
//
typedef struct ir_sensor {
	const signed short a;
	const signed short b;
	const signed short k;
} ir_sensor;

typedef struct distanzSensorWerte {
	int AnalogWert;
	unsigned long letzteMessung;
	int Distanz;
} distanzSensorWerte;

typedef struct servoWerte {
	boolean attached;
	int messrichtung;
	int delta;
} senrvoWerte;

extern int loopCount;

// funktionen
void setupDistanzMessung();

void ermittleDistanzen(MOVEMENT Richtung);

int freieFahrt(MOVEMENT Richtung);

