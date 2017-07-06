/*
 * [kiwi] Indicated Area Prediction System for Shopping
 * - Record Board
 * @author Thais Watanabe
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// records and leds ids
byte ledsPins[]={2,4,5,12,13,14,15,16};
char* recordsIds[]={"59546f948a99e30e6ff54be2","59546fec8a99e30e6ff54be3",
                    "595470348a99e30e6ff54be4","5954706b8a99e30e6ff54be5"};

// router ssid & pass
const char* ssid = "";
const char* password = "";

// node.js server info
const char* host = "192.168.11.2";
const int port = 3000;

// clinet
WiFiClient client;

// variables
String record = "";

// working with esp-wroom
extern "C" {
#include "user_interface.h"
}

void setup() {
	Serial.begin(9600);

  // initializing leds
	for (int i=0; i<8; i++) {
		pinMode(ledsPins[i],OUTPUT);
	}

  // method to connect to wifi
	connectingToWiFi();
}

void loop() {
  // connection with nodejs server
	connectingToServer();
  // discover which record is selected
	whichRecord();
  // then light the right leds
	enableLEDs();

	delay(1000);
}

/*
 * Make connection with wifi
 */
void connectingToWiFi() {
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println(" ");
	Serial.print("Connected with IP address: ");
	Serial.println(WiFi.localIP());
}

/*
 * Make connection with server
 */
void connectingToServer() {
	if (!client.connect(host, port)) {
		Serial.println("Connection failed!");
		delay(1000);
    //ESP.deepSleep(2 * 1000 * 1000);
    //delay(1000);
		return;
	}
}

/*
 * Logic to light up the leds
 */
void enableLEDs() {

  // turn off all the leds
	for (int led=0; led<8; led++) {
		digitalWrite(ledsPins[led], LOW);
	}

  // verify which one to light up
	for (int i=0; i<4; i++) {
		if (record == recordsIds[i]) {
			digitalWrite(ledsPins[i], HIGH);
		}
	}
}

/*
 * Discover which record is selected
 */
void whichRecord() {
	String url = "/whichRecord";

  // getting the information
	client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
	delay(10);

	while(client.available()) {
		record = String(client.readStringUntil('\r'));
		record.trim();
		Serial.println(record);
	}
}
