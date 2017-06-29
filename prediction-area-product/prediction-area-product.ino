/*
 * [kiwi] Indicated Area Prediction System for Shopping
 * - Record Board
 * @author Thais Watanabe
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// shift register
int latchPin = 12;
int clockPin = 2;
int dataPin = 14;

byte leds = 0;
int currentLED = 0;

// records unique id
const char* recordOneId = "59546f948a99e30e6ff54be2";
const char* recordTwoId = "59546fec8a99e30e6ff54be3";

// router ssid & pass
const char* ssid = "";
const char* password = "";

// node.js server info
const char* host = "";
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

  //set pins to output to control the shift register
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

  // initialize the leds
	leds = 0;

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
  leds = 0;

  // the first record
	if(record == recordOneId) {
		for (int i=4; i<8; i++) {
			bitSet(leds, i);
		}
		digitalWrite(latchPin, LOW);
		shiftOut(dataPin, clockPin, LSBFIRST, leds);
		digitalWrite(latchPin, HIGH);

  // the second record
	} else if(record == recordTwoId) {
		for (int i=0; i<4; i++) {
			bitSet(leds, i);
		}
		digitalWrite(latchPin, LOW);
		shiftOut(dataPin, clockPin, LSBFIRST, leds);
		digitalWrite(latchPin, HIGH);

  // when the record does not exist
	} else {
    for (int i=0; i<8; i++) {
			bitClear(leds, i);
		}
		digitalWrite(latchPin, LOW);
		shiftOut(dataPin, clockPin, LSBFIRST, leds);
		digitalWrite(latchPin, HIGH);
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
