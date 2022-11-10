/*
  # Display JSON from Server
  by Leo Mancini
  
  # HTTP Request
  Based on tutorial by Rui Santos
  https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/
  Permission is hereby granted, free of charge, to any
  person obtaining a copy of this software and associated
  documentation files. The above copyright notice and this
  permission notice shall be included in all copies or
  substantial portions of the Software.

  # OLED Display 
  Written by Limor Fried/Ladyada for Adafruit Industries,
  with contributions from the open source community.
  BSD license, check license.txt for more information
  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source
  hardware by purchasing products from Adafruit!
  All text above, and the splash screen below must be
  included in any redistribution.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
#define SCREEN_ADDRESS  0x3C
#define LED_001            14

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "";
const char* password = "";
const char* dataURL = "";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

String data;
String dataArr[3];

void setup() {
  Serial.begin(115200);
  
  /* DATA */
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  
  /* DISPLAY */
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
 
  display.display();

  /* LED */
  pinMode(LED_001, OUTPUT); 
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED){

      /* DATA */
      data = httpGETRequest(dataURL);
      Serial.println(data);
      JSONVar json = JSON.parse(data);

      if (JSON.typeof(json) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(json);
    
      JSONVar keys = json.keys();

      /* DISPLAY */
      String jsonString = JSON.stringify(json["display"]["text"]);
      jsonString.replace("<br>", "\n");
      jsonString.replace("\"", "");

      Serial.println(jsonString);
      
      printDisplay(jsonString, json["display"]["size"]);

      /* LED */
      String led1Status = JSON.stringify(json["led"]["1"]);
      led1Status.replace("\"", "");
      
      if (led1Status == "on") {
        digitalWrite(LED_001, HIGH);
      } else if (led1Status == "off") {
        digitalWrite(LED_001, LOW);
      }
    } else {
      Serial.println("WiFi Disconnected");
    }
    
    lastTime = millis();
  }
}

String httpGETRequest(const char* dataURL) {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, dataURL);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();

  return payload;
}

void printDisplay(String string, int size) {
  display.clearDisplay();

  display.setCursor(0,0);
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);
  display.println(string);
  
  display.display();
  delay(2000);
}
