// Move.h

#ifndef _MOVE_h
#define _MOVE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
enum MOTORS { FRONT_RIGHT, FRONT_LEFT, BACK_RIGHT, BACK_LEFT };

// Initialisierung der Bewegungsteile
void setupMove();

// folge dem Joystick
void followJoystick();

#endif