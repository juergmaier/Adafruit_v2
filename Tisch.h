#pragma once

// Tisch
int Pin_SpeedM1 = 8;
int Pin_UpDownM1 = 26;

int Pin_SpeedM2 = 9;
int Pin_UpDownM2 = 27;

int pinEnablePower = 28;
int pinHeight = A10;

int speed1 = 200;	// die Geschwindigkeiten der beiden Motoren damit sie parallel fahren
int speed2 = 245;

void setup_Tisch();

void tischUp(float target);
void tischDown(float target);
void tischStopp();

