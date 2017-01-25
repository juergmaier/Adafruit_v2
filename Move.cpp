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

int joyX = 9;  // Arduino Pin
int joyY = 8;  // Arduino Pin
int joyZ = 52; // Arduino Pin 

int cartSpeed = 0;  // default
int prevCartSpeed = 0;
double maxSpeedIncrease = 500 * rate;

int XPos;
int YPos;
int ZPos;
float Richtung;
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

	AFMS.begin();

	// Druck-Schalter JoyStick als Input
	pinMode(joyZ, INPUT_PULLUP);
	digitalWrite(joyZ, HIGH);

}


void followJoystick() {

// den joystick lesen
XPos = int((analogRead(joyX) - 517) / 10);
YPos = int((analogRead(joyY) - 513) / 10);
ZPos = digitalRead(joyZ);

// Ruhezone Zenter
if (abs(XPos) < 10) XPos = 0;
if (abs(YPos) < 10) YPos = 0;

Richtung = degrees(atan2(XPos, YPos));

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

		if (Richtung > -22.5   && Richtung <= -22.5 + 45)   moveCart(VORWAERTS);
		if (Richtung > 22.5    && Richtung <= 22.5 + 45)    moveCart(VOR_DIAG_RECHTS);
		if (Richtung > 67.5    && Richtung <= 67.5 + 45)    moveCart(RECHTS);
		if (Richtung > 112.5   && Richtung <= 112.5 + 45)   moveCart(RUECK_DIAG_RECHTS);
		if (Richtung > 157.5 || Richtung <= -157.5)       moveCart(RUECKWAERTS);
		if (Richtung > -157.5  && Richtung <= -157.5 + 45)  moveCart(RUECK_DIAG_LINKS);
		if (Richtung > -112.5  && Richtung <= -112.5 + 45)  moveCart(LINKS);
		if (Richtung > -67.5   && Richtung <= -67.5 + 45)   moveCart(VOR_DIAG_LINKS);
	}
	else { // gedrückte Drehtaste
		if (Richtung > 0) moveCart(DREHEN_RECHTS);
		if (Richtung < 0) moveCart(DREHEN_LINKS);
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

	int Freiheit = freieFahrt(Richtung);

	fr->run(moves[Richtung][FRONT_RIGHT]);
	fl->run(moves[Richtung][FRONT_LEFT]);
	br->run(moves[Richtung][BACK_RIGHT]);
	bl->run(moves[Richtung][BACK_LEFT]);
}

void stop() {
	move = "stop";
	prevCartSpeed = 0;
	fr->run(RELEASE);
	fl->run(RELEASE);
	br->run(RELEASE);
	bl->run(RELEASE);
}
