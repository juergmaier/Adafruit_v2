// Distance.h

#ifndef _DISTANCE_h
#define _DISTANCE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif

//#include "Move.h"

//
// Die Struktur der Parameter des Infrarot-Entfernungsmessers
//
typedef struct ir_sensor {
	const signed short a;
	const signed short b;
	const signed short k;
} ir_sensor;

typedef struct distanzSensor {
	int AnalogWert;
	unsigned long letzteMessung;
	unsigned long nextFullScanDone;
	signed short Distanz;
	boolean Hindernis;
	boolean Abgrund;
} distanzSensor;

void setupDistanzMessung();

//int freieFahrt(MOVEMENT Richtung);

