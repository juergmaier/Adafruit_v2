#include "Tisch.h"

void setup_Tisch() {

	pinMode(pinEnablePower, OUTPUT);
	digitalWrite(pinEnablePower, LOW);

	pinMode(Pin_SpeedM1, OUTPUT);
	analogWrite(Pin_SpeedM1, 0);
	pinMode(Pin_UpDownM1, OUTPUT);

	pinMode(Pin_SpeedM2, OUTPUT);
	analogWrite(Pin_SpeedM2, 0);
	pinMode(Pin_UpDownM2, OUTPUT);
}

float aktuelleTischHoehe() {

	int RawHeight = analogRead(pinHeight);

	// die ungefähre aktuelle obere Höhe des Tisches (ab Boden)
	return 30;
}

void tischStopp() {
	analogWrite(Pin_SpeedM1, 0);
	analogWrite(Pin_SpeedM2, 0);
	digitalWrite(pinEnablePower, LOW);  // cut table motor power through relais
}

void tischUp(float target) {

	if (aktuelleTischHoehe() > target) tischStopp();

	Serial.println("tisch nach oben fahren");
	// direction
	digitalWrite(Pin_UpDownM1, HIGH);
	digitalWrite(Pin_UpDownM2, HIGH);

	analogWrite(Pin_SpeedM1, speed1); // die Motoren fahren mit gleicher Speed nicht parallel
	analogWrite(Pin_SpeedM2, speed2);

	digitalWrite(pinEnablePower, HIGH);  // table motor controller power on

}



void tischDown(float target) {

	if (aktuelleTischHoehe() < target) tischStopp();

	Serial.println("tisch nach unten fahren");

	// direction
	digitalWrite(Pin_UpDownM1, LOW);
	digitalWrite(Pin_UpDownM2, LOW);

	analogWrite(Pin_SpeedM1, speed1); // die Motoren fahren mit gleicher Speed nicht parallel
	analogWrite(Pin_SpeedM2, speed2);

	digitalWrite(pinEnablePower, HIGH);  // table motor controller power on
}

