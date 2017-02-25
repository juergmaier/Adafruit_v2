// 
// 
// 

#include "Move.h"
#include "Distance.h"


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *fl = AFMS.getMotor(1);
Adafruit_DCMotor *fr = AFMS.getMotor(2);
Adafruit_DCMotor *br = AFMS.getMotor(3);
Adafruit_DCMotor *bl = AFMS.getMotor(4);

int joyX = A15;  // Arduino Pin Joystick x
int joyY = A13;  // Arduino Pin Joystick y
int joyZ = 52;   // Arduino Pin Joystick pushdown

int cartSpeed = 0;  // default
int prevCartSpeed = 0;


int XPos;
int YPos;
int ZPos;
float fahrRichtung;
float oldFahrRichtung = 999;

int maxSpeedIncrease = 1;
String move;
String prevMove;


String moveName[10] = { "vor","vor_diag_rechts","vor_diag_links","links","rechts","rueckwaerts","rueck_diag_rechts","rueck_diag_links","drehen_links","drehen_rechts" };

int moves[10][4]{
	/* VORWAERTS         */{ FORWARD,  FORWARD,  FORWARD,  FORWARD },
	/* VOR_DIAG_RECHTS   */{ RELEASE,  FORWARD,  FORWARD,  RELEASE },
	/* VOR_DIAG_LINKS    */{ FORWARD,  RELEASE,  RELEASE,  FORWARD },
	/* LINKS             */{ BACKWARD, FORWARD,  FORWARD,  BACKWARD },
	/* RECHTS            */{ FORWARD,  BACKWARD, BACKWARD, FORWARD },
	/* RUECKWAERTS       */{ BACKWARD, BACKWARD, BACKWARD, BACKWARD },
	/* RUECK_DIAG_RECHTS */{ BACKWARD, RELEASE,  RELEASE,  BACKWARD },
	/* RUECK_DIAG_LINKS  */{ RELEASE,  BACKWARD, BACKWARD, RELEASE },
	/* DREHEN_LINKS      */{ FORWARD,  BACKWARD, FORWARD,  BACKWARD },
	/* DREHEN_RECHTS     */{ BACKWARD, FORWARD,  BACKWARD, FORWARD }
};


void setupMove() {

	maxSpeedIncrease = 1.0 / ANZ_MESSUNGEN_PRO_SCAN * 500;
	Serial.print("maxSpeedIncrease "); Serial.println(maxSpeedIncrease);

	AFMS.begin();

	// Druck-Schalter JoyStick als Input
	pinMode(joyZ, INPUT_PULLUP);
	digitalWrite(joyZ, HIGH);

}



void stop() {
	move = "stop";
	prevCartSpeed = 0;
	fr->run(RELEASE);
	fl->run(RELEASE);
	br->run(RELEASE);
	bl->run(RELEASE);

	tischStop();

}

void setCartSpeed() {

	// limit amount of acceleration
	if (cartSpeed > prevCartSpeed + maxSpeedIncrease) {
		cartSpeed = prevCartSpeed + maxSpeedIncrease;
		Serial.println("acceleration ramp");
	}
	prevCartSpeed = cartSpeed;
	fr->setSpeed(cartSpeed);
	fl->setSpeed(cartSpeed);
	br->setSpeed(cartSpeed);
	bl->setSpeed(cartSpeed);
}

void moveCart(MOVEMENT Richtung) {

	move = moveName[Richtung];

	ermittleDistanzen(Richtung);

	int Freiheit = freieFahrt(Richtung);

	if (Freiheit > 20) {
		fr->run(moves[Richtung][FRONT_RIGHT]);
		fl->run(moves[Richtung][FRONT_LEFT]);
		br->run(moves[Richtung][BACK_RIGHT]);
		bl->run(moves[Richtung][BACK_LEFT]);
	} 
	else {
		Serial.print("Hindernis/Abgrund in Sicht, STOPP, Freiheit: "); Serial.println(Freiheit);
		stop();
	}

}


void followJoystick() {

// den joystick lesen
XPos = int((analogRead(joyX) - 517) / 10);
YPos = int((analogRead(joyY) - 513) / 10);
ZPos = digitalRead(joyZ);

// Ruhezone Zenter
if (abs(XPos) < 10) XPos = 0;
if (abs(YPos) < 10) YPos = 0;

fahrRichtung = degrees(atan2(XPos, YPos));

if (fahrRichtung != oldFahrRichtung) Serial.print("Joystick Richtung degrees "); {
	oldFahrRichtung = fahrRichtung;
	Serial.println(fahrRichtung);
}

// benutze den grösseren Ausschlag des Joystick als Geschwindigkeit-Vorgabe
if (abs(XPos) > abs(YPos)) {
	cartSpeed = map(abs(XPos), 0, 51, 0, 150);  // reduziere Geschwindigkeit von Seitwärts-Bewegungen (150 max)
}
else {
	cartSpeed = map(abs(YPos), 0, 51, 0, 240);
}
setCartSpeed();

if (cartSpeed == 0) stop();

else {
	
	if (ZPos == 1) {  // Drehung nicht aktiv

		if (fahrRichtung > -22.5   && fahrRichtung <= -22.5 + 45)   moveCart(VORWAERTS);
		if (fahrRichtung > 22.5    && fahrRichtung <= 22.5 + 45)    moveCart(VOR_DIAG_RECHTS);
		if (fahrRichtung > 67.5    && fahrRichtung <= 67.5 + 45)    moveCart(RECHTS);
		if (fahrRichtung > 112.5   && fahrRichtung <= 112.5 + 45)   moveCart(RUECK_DIAG_RECHTS);
		if (fahrRichtung > 157.5   || fahrRichtung <= -157.5)       moveCart(RUECKWAERTS);
		if (fahrRichtung > -157.5  && fahrRichtung <= -157.5 + 45)  moveCart(RUECK_DIAG_LINKS);
		if (fahrRichtung > -112.5  && fahrRichtung <= -112.5 + 45)  moveCart(LINKS);
		if (fahrRichtung > -67.5   && fahrRichtung <= -67.5 + 45)   moveCart(VOR_DIAG_LINKS);
	}
	else { // gedrückte Drehtaste
		if (fahrRichtung > 67.5    && fahrRichtung <= 67.5 + 45)    moveCart(DREHEN_RECHTS);
		if (fahrRichtung > -112.5  && fahrRichtung <= -112.5 + 45)  moveCart(DREHEN_LINKS);

		if (fahrRichtung > -22.5   && fahrRichtung <= -22.5 + 45)   tischUp(50);
		if (fahrRichtung > 157.5   || fahrRichtung <= -157.5)       tischDown(20);

	}
}

// log wenn Richtung ändert
if (move != prevMove) {
	if (prevMove != "stop") {
		stop();
		delay(3 * prevCartSpeed);   // keine aprupten Richtungswechsel
	}
	Serial.print(prevMove); Serial.print(" -> "); Serial.println(move);
	prevMove = move;

}

}


