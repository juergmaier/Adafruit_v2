#include "Move.h"
#include "Distance.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"


typedef enum MOVEMENT {
	VORWAERTS, VOR_DIAG_RECHTS, VOR_DIAG_LINKS,
	LINKS, RECHTS,
	RUECKWAERTS, RUECK_DIAG_RECHTS, RUECK_DIAG_LINKS,
	DREHEN_LINKS, DREHEN_RECHTS
} MOVEMENT;


float rate = 10;  // loop cycle/s

void setup() {

  setupMove();

  setupDistanzMessung();

  Serial.begin(115200);
}

void loop() {

	followJoystick();

	delay(1/rate * 1000);	// ms
}