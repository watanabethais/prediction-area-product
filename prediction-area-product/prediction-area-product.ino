/*
 * Indicated Area Prediction System for Shopping
 * - Product LED
 * @author Thais Watanabe
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define LED 13

// router ssid & pass
const char* ssid = "KawalabStudy-g";
const char* password = "kawalab0621";

// node.js server info
const char* host = "192.168.11.8";
const int port = 3000;

// clinet
WiFiClient client;

// variables
String product = "";

// working with esp-wroom
extern "C" {
#include "user_interface.h"
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  connectingToWiFi();
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

void loop() {
  connectingToServer();
  whichProduct();

  if(product == "product1") {
    digitalWrite(LED, HIGH);
    Serial.println(product);
    Serial.println("This is the product!");
  } else {
    digitalWrite(LED, LOW);
    Serial.println(product);
    Serial.println("This is not the product!");
  }

  delay(1000);
}

void whichProduct() {
  String url = "/whichProduct";

  // getting the information
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  delay(10);

  while(client.available()) {
    product = String(client.readStringUntil('\r'));
    product.trim();
    Serial.println(product);
  }
}
