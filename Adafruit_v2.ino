#include "Move.h"
#include "Distance.h"
#include "Tisch.h"

#include <Wire.h>
//#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_MS_PWMServoDriver.h"



int loopCount;

void setup() {

  setupMove();

  setupDistanzMessung();

  setupTisch();

  Serial.begin(115200);

  Serial.println("adafruit_v2.setup done");
}

void loop() {

	// loopcount zählt durch die einzelnen Servoschritte
	loopCount++;
	loopCount = loopCount%ANZ_MESSUNGEN_PRO_SCAN;

	followJoystick();

	//delay((1/rate) * 1000);	// ms
	delay(2000);
}