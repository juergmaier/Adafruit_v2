// 

#include "Distance.h"
#include <Servo.h>

#define min(a,b) ((a)<(b)?(a):(b))

///////////////////////
// Distanz Sensor Front
///////////////////////
int PinScanServoVR = 50;
int PinDistanzVorneNah = A11;       // A12
int PinDistanzVorneFern = A12;       // A11

Servo ScanServoVR;
int ScanRate = 2000;	// 2000 ms für einen swipe


//
// Die Parametereigenschaften der Sensoren
// 
//ir_sensor GP2Y0A21YK = { 5461, -17, 2 };
ir_sensor GP2Y0A21YK = { 10000, -17, 2 };


distanzSensor vrNah;	//A41
distanzSensor vrFern;	//A21

distanzSensor vlNah;	//A41

/////////////////////////
// Messrichtungen Distanz
/////////////////////////
int neueMessrichtung = 1500;    // PWM geradeaus
int Delta = 50;            // PWM Aenderung
int MessrichtungMin = 800;
int MessrichtungMax = 2200;
int Distanz;

void setupDistanzMessung() {
	pinMode(PinScanServoVR, OUTPUT);
	ScanServoVR.attach(PinScanServoVR);
}

int freieFahrt(MOVEMENT Richtung) {

	if (Richtung == VORWAERTS) {
		if (millis() - vrNah.letzteMessung > ScanRate) {
			if (millis() > vrNah.nextFullScanDone) {
				return min(vrNah.Distanz, vlNah.Distanz);
			} else {
				vrNah.nextFullScanDone = millis() + ScanRate;
			}
		}
			
	}
	return -1;
}

// distanz 021 20..80 cm
// Konvertieren der Werte des Infrarot-Entfernungsmessers in Centimeter 
// Gibt -1 aus, wenn die Konvertierung nicht erfolgreich war
//
int distanzFern021(struct ir_sensor sensor,
	int adc_value)
{
	if (adc_value + sensor.b <= 0)
	{
		return -1;
	}

	return sensor.a / (adc_value + sensor.b) - sensor.k;
}

// distanz 041 10..30 cm
// Konvertieren der Werte des Infrarot-Entfernungsmessers in Centimeter 
// Gibt -1 aus, wenn die Konvertierung nicht erfolgreich war
int distanzNah041(int adc_value)
{
	if (adc_value > 600 || adc_value < 100)
	{
		return -1;
	}

	return int(2 / (0.000413153 * adc_value - 0.0055266887));
}


/////////////////////////
/////////////////////////
void Messrichtung() {

	neueMessrichtung += Delta;
	if (neueMessrichtung > MessrichtungMax) {
		Delta = -Delta;
		neueMessrichtung = MessrichtungMax + Delta;
	}
	if (neueMessrichtung < MessrichtungMin) {
		Delta = -Delta;
		neueMessrichtung = MessrichtungMin + Delta;
	}
	ScanServoVR.write(neueMessrichtung);
	//Serial.print("neue Messrichtung: "); Serial.println(neueMessrichtung);
}


/////////////////////////
/////////////////////////
void Distanzen(MOVEMENT Richtung) {

	if (Richtung > -90 && Richtung < 90) {   // Richtung vorwärts
		vrNah.AnalogWert = analogRead(PinDistanzVorneNah);

		// berechnen der metrischen Distanz
		vrNah.Distanz = distanzNah041(vrNah.AnalogWert);
		Serial.print("Distanzwert nah: ");
		Serial.print(vrNah.Distanz);
		Serial.print("  Raw: ");
		Serial.print(vrNah.AnalogWert);

		if (vrNah.Distanz > 0) {
			vrNah.Hindernis = vrNah.Distanz < 20;
			vrNah.Abgrund = vrNah.Distanz > 30;
			//	Serial.print("  Hindernis: "); Serial.print(vrNah.Hindernis);
			//	Serial.print("  Abgrund: "); Serial.print(vrNah.Abgrund);
		}
		//Serial.println();

		vrFern.AnalogWert = analogRead(PinDistanzVorneFern);
		vrFern.Distanz = distanzFern021(GP2Y0A21YK, vrFern.AnalogWert);

		Serial.print("  Distanzwert fern: "); Serial.print(vrFern.Distanz);
		Serial.print("  Raw: "); Serial.print(vrFern.AnalogWert);

		if (vrFern.Distanz > 0) {
			vrFern.Hindernis = vrFern.Distanz > 300;
			vrFern.Abgrund = vrFern.Distanz < 190;
			//	Serial.print("  Hindernis: "); Serial.print(vrFern.Hindernis);
			//	Serial.print("  Abgrund: "); Serial.print(vrFern.Abgrund);
		}
		Serial.println();
	}

	Messrichtung();
}

