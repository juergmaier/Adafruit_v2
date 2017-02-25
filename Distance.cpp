// 
#include "Adafruit_v2.h"
#include "Distance.h"
#include <Servo.h>

#define min(a,b) ((a)<(b)?(a):(b))


const int ScanRate = 2000;	// 2000 ms für einen swipe
const int ScanSchritt = 14;		// Gradabweichung zwischen Messungen
const int MessrichtungMin = -50;
const int MessrichtungMax = 50;


// die ScanServos
enum SCAN_SERVOS {
	VL, VR, LM, RM, HL, HR, SCAN_SERVOS_COUNT
};
Servo Servos[SCAN_SERVOS_COUNT];

typedef struct servoDefinition {
	int servoID;
	char servoName[3];
	boolean installed;
	int servoPin;
} servoDefinition;

servoDefinition servoDefinitionen[SCAN_SERVOS_COUNT]{
	{ VL, "VL", true, 2 },
	{ VR, "VR", true, 3 },
	{ LM, "LM", true, 4 },
	{ RM, "RM", false, 5 },
	{ HL, "HL", true, 6 },
	{ HR, "HR", true, 7 }
};

servoWerte servoDaten[SCAN_SERVOS_COUNT];

//
// Die Parametereigenschaften der Sensoren
// 
//ir_sensor GP2Y0A21YK = { 5461, -17, 2 };
ir_sensor GP2Y0A21YK = { 5461, -17, 2 };

enum DISTANZ_SENSOREN {
	VL_NAH, VL_FERN, VR_NAH, VR_FERN,
	LM_NAH, LM_FERN, RM_NAH, RM_FERN,
	HL_NAH, HL_FERN, HR_NAH, HR_FERN, 
	DISTANZ_SENSOREN_COUNT
};


enum SENSOR_TYPEN { A41, A21, SENSOR_TYPEN_COUNT };		// Nah, Fern
enum SENSOR_INFO { SENSOR_ID, SENSOR_TYPE, SENSOR_PIN, SWIPE_SERVO_ID, SENSOR_INFO_COUNT };

typedef struct sensorDefinition {
	int sensorID;
	char sensorName[8];
	SENSOR_TYPEN sensorTyp;
	boolean installed;
	int sensorPin;
	int servoID;
} sensorDefinition;

sensorDefinition sensorDefinitionen[DISTANZ_SENSOREN_COUNT]{
	{ VL_NAH,  "VL_NAH ", A41, true, A0, VL },
	{ VL_FERN, "VL_FERN", A21, false, A1, VL },
	{ VR_NAH,  "VR_NAH ", A41, true, A2, VR },
	{ VR_FERN, "VR_FERN", A21, true, A3, VR },
	{ LM_NAH,  "LM_NAH ", A21, false, A4, LM },
	{ LM_FERN, "LM_FERN", A21, false, A4, LM },
	{ RM_NAH,  "RM_NAH ", A21, false, A5, RM },
	{ RM_FERN, "RM_FERN", A21, false, A5, RM },
	{ HL_NAH,  "HL_NAH ", A41, true, A8, HL },
	{ HL_FERN, "HL_FERN", A21, true, A9, HL },
	{ HR_NAH,  "HR_NAH ", A41, true, A10, HR },
	{ HR_FERN, "HR_FERN", A21, false, A11, HR }
};



int richtungSensorZuordnung[MOVEMENT_COUNT][4]{
	/* VORWAERTS         */{ VL_NAH, VL_FERN, VR_NAH, VR_FERN },
	/* VOR_DIAG_RECHTS   */{ VR_NAH, VR_FERN, LM_NAH, LM_FERN },
	/* VOR_DIAG_LINKS    */{ VL_NAH, VL_FERN, LM_NAH, LM_FERN },
	/* LINKS             */{ VL_NAH, VL_FERN, LM_NAH, LM_FERN },
	/* RECHTS            */{ VR_NAH, VR_FERN, HR_NAH, HR_FERN },
	/* RUECKWAERTS       */{ HL_NAH, HL_FERN, HR_NAH, HR_FERN },
	/* RUECK_DIAG_RECHTS */{ HR_NAH, HR_FERN, RM_NAH, RM_FERN },
	/* RUECK_DIAG_LINKS  */{ HL_NAH, HL_FERN, LM_NAH, LM_FERN },
	/* DREHEN_LINKS      */{ LM_NAH, HL_NAH,  RM_NAH, VL_NAH  },
	/* DREHEN_RECHTS     */{ VL_NAH, RM_NAH,  HR_NAH, LM_NAH  }
};

distanzSensorWerte sensorDaten[DISTANZ_SENSOREN_COUNT][ANZ_MESSUNGEN_PRO_SCAN];


void setupDistanzMessung() {
	for (int i = 0; i <= SCAN_SERVOS_COUNT; i++) {
		int Pin = servoDefinitionen[i].servoPin;
		if (servoDefinitionen[i].installed) pinMode(Pin, OUTPUT);

		servoDaten[i].delta = ScanSchritt;
	};
}


// distanz 021 20..80 cm
// Konvertieren der Werte des Infrarot-Entfernungsmessers in Centimeter 
// Gibt -1 aus, wenn kein sinnvoller Messwert vorhanden ist
//
int distanz021(ir_sensor sensor,
	int adc_value)
{
	if (adc_value < 120 || adc_value > 400)
	{
		return 200;	//keine Messung möglich
	}

	return sensor.a / (adc_value + sensor.b) - sensor.k;
}

// distanz 041 10..30 cm
// Konvertieren der Werte des Infrarot-Entfernungsmessers in Centimeter 
// Gibt 200 aus, wenn die Konvertierung nicht erfolgreich war
int distanz041(int adc_value)
{
	if (adc_value > 600 || adc_value < 100)
	{
		return 200;
	}

	return int(2 / (0.000413153 * adc_value - 0.0055266887));
}


/////////////////////////
/////////////////////////
void neueMessrichtung(SCAN_SERVOS servoID) {

	if (!servoDefinitionen[servoID].installed) return;

	servoDaten[servoID].messrichtung += servoDaten[servoID].delta;

	if (servoDaten[servoID].messrichtung > MessrichtungMax) {
		servoDaten[servoID].delta = -ScanSchritt;
	}
	if (servoDaten[servoID].messrichtung < MessrichtungMin) {
		servoDaten[servoID].delta = ScanSchritt;
	}

	int PWM_Wert = servoDaten[servoID].messrichtung * (2000 / 150) + 1500;

	if (!servoDaten[servoID].attached) {
		Servos[servoID].attach(servoDefinitionen[servoID].servoPin);
		servoDaten[servoID].attached = true;
		Serial.print("Servo attached "); Serial.print(servoDefinitionen[servoID].servoName);
	}
	Serial.print("servo "); Serial.print(servoDefinitionen[servoID].servoName);
	Serial.print(" neue Messrichtung PWM "); Serial.print(PWM_Wert); 
	Serial.print(" messrichtung "); Serial.println(servoDaten[servoID].messrichtung);
	Servos[servoID].write(PWM_Wert);

}

// finde älteste Messung im Buffer
boolean alteMessdaten(int sensorID) {

	unsigned long aeltesteMesszeit;
	for (int i = 0; i < ANZ_MESSUNGEN_PRO_SCAN; i++) {
		aeltesteMesszeit = min(aeltesteMesszeit, sensorDaten[sensorID][i].letzteMessung);
	}
	unsigned long alter = millis() - aeltesteMesszeit;
	Serial.print("alter Messdaten "); Serial.println(alter);
	return (alter > 20000);
}

// finde kleinste Distanz im Buffer
int kleinsteDistanz(int sensorID) {

	//Serial.println("kleinsteDistanz ");

	int kleinsterWert = 100;
	for (int i = 0; i < ANZ_MESSUNGEN_PRO_SCAN; i++) {
		if (sensorDaten[sensorID][i].Distanz > 0) {
			kleinsterWert = min(kleinsterWert, sensorDaten[sensorID][i].Distanz); 
		}

		//Serial.print("Sensor "); Serial.print(sensorDefinitionen[sensorID].sensorName);
		//Serial.print(" Richtung "); Serial.print(i);
		//Serial.print(" Distanz "); Serial.println(sensorDaten[sensorID][i].Distanz);
		

	}
	//Serial.print(" kleinsteDistanz.kleinsterWert "); Serial.println(kleinsterWert);
	return kleinsterWert;
}

/////////////////////////
/////////////////////////
void ermittleDistanzen(MOVEMENT Richtung) {

	for (int i = 0; i <= 3; i++) {

		int sensorID = richtungSensorZuordnung[Richtung][i];
		sensorDefinition *p_sensor = &sensorDefinitionen[sensorID];

		// es werden 2 sensoren abgefragt pro servo
		if (i % 2 == 0) neueMessrichtung(p_sensor->servoID);

		îf(sensorDefinitionen[sensorID].installed) {

			sensorDaten[sensorID][loopCount].AnalogWert = analogRead(p_sensor->sensorPin);

			if (p_sensor->sensorTyp == A41) {
				sensorDaten[sensorID][loopCount].Distanz = distanz041(sensorDaten[sensorID][loopCount].AnalogWert);

			}

			if (p_sensor->sensorTyp == A21) {
				sensorDaten[sensorID][loopCount].Distanz = distanz021(GP2Y0A21YK, sensorDaten[sensorID][loopCount].AnalogWert);
			}
		}
		else {
			sensorDaten[sensorID][loopCount].Distanz = 200;
		}

		sensorDaten[sensorID][loopCount].letzteMessung = millis();		// Zeitpunkt der Messung

		//Serial.print("Distanzsensor "); Serial.print(p_sensor->sensorName);
		//Serial.print(" Analogwert: ");  Serial.print(sensorDaten[sensorID][loopCount].AnalogWert);
		//Serial.print(" Distanz: "); Serial.println(sensorDaten[sensorID][loopCount].Distanz);

	}
}


// gibt die ungefähre Distanz in cm zum nächsten Hindernis / Abgrund in Fahrtrichtung
int freieFahrt(MOVEMENT Richtung) {

	// stelle sicher dass die betroffenen Sensoren aktuelle Daten haben
	int kleinsteFreiheit = 100;

	//for (int i = 0; i <= 3; i++) {
	for (int i = 1; i <= 1; i++) {
	
		int sensorID = richtungSensorZuordnung[Richtung][i];
		//Serial.print("freieFahrt "); Serial.println(sensorID);

		boolean messdatenSindAlt = alteMessdaten(sensorID);
		if (messdatenSindAlt) {
			kleinsteFreiheit = -1;
		}
		else {
			kleinsteFreiheit = min(kleinsteFreiheit, kleinsteDistanz(sensorID));
			//Serial.print("kleinste Distanz "); Serial.println(kleinsteDistanz(sensorID));
		}
	}

	Serial.print("kleinste Freiheit "); Serial.println(kleinsteFreiheit);
	return kleinsteFreiheit;
}
