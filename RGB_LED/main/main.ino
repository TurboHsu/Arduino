/*
    RGB Clock
    Author TurboHsu
      LED Counts: 12
      LED Connect -> D8
      Button Connect -> D5
      Digit CLK -> D1
      Digit DIO -> D2
    Version 0.0.2
                    For CY
*/
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Arduino.h>
#include <TM1637Display.h>


//Config
#define NUM_LEDS 12
#define DATA_PIN 15
#define DELAY_MS 5
CRGB leds[NUM_LEDS];  
const char *wifiSsid = "";
const char *wifiPassword = "";
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"ntp1.aliyun.com",60*60*8,30*60*1000);
#define CLK 5
#define DIO 4
TM1637Display digiDisplay(CLK, DIO);

//Vars
int ledRGBCount , ledRGBState , RGBMode;
unsigned long int lastSysTimeLED , lastSysTimeBTN , lastSysTimeCLK , lastSysTimeUpdate;
bool isChangedBTN , isCLKPointON;
uint8_t displayTime[4];

void setup() {
  Serial.begin(115200);
  while (!Serial)
        continue;

  pinMode(14, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  digiDisplay.setBrightness(0x0f);

  Serial.println("[I] TurboCloud IOT System started. \n   Application: RGB Clock\n   Version: v0.0.1");

  Serial.printf("[I] Connecting to WIFI [%s]...\n",wifiSsid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
      for (int i = 0; i <= 11; i++) {
        delay(30);
        leds[i] = CRGB(255, 255, 255);
        FastLED.show();
      }
      digitalWrite(LED_BUILTIN, LOW);
      for (int i = 0; i <= 11; i++) {
        delay(30);
        leds[i] = CRGB(0, 0, 0);
        FastLED.show();
      }
      digitalWrite(LED_BUILTIN, HIGH);   
  }
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("[I] WIFI Connected!");
  
  timeClient.update();
  displayTime[0] = digiDisplay.encodeDigit(timeClient.getHours() / 10);
  displayTime[1] = digiDisplay.encodeDigit(timeClient.getHours() % 10);
  displayTime[2] = digiDisplay.encodeDigit(timeClient.getMinutes() / 10);
  displayTime[3] = digiDisplay.encodeDigit(timeClient.getMinutes() % 10);
  digiDisplay.setSegments(displayTime, 4, 0);

  lastSysTimeLED = millis();
  lastSysTimeBTN = millis();
  lastSysTimeCLK = millis();
  lastSysTimeUpdate = millis();
  isCLKPointON = false;  
  ledRGBCount = 0;
  ledRGBState = 0;
  RGBMode = 0;
}

void loop() {
  timeUpdate();
  ledDisplay();
  buttonDetect();
  clockDisplay();
}

void timeUpdate() {
  if (lastSysTimeUpdate + 3600000 <= millis()) { //One hour once
    timeClient.update();
    lastSysTimeUpdate = millis();
  } else if (lastSysTimeUpdate > millis()) { //After millis reset, lastSysTimeLED must be bigger.
    lastSysTimeUpdate = millis();
  }
}

void clockDisplay() {
  if (lastSysTimeCLK + 1000 <= millis()) {
      lastSysTimeCLK = millis();
      displayTime[0] = digiDisplay.encodeDigit(timeClient.getHours() / 10);
      displayTime[1] = digiDisplay.encodeDigit(timeClient.getHours() % 10);
      displayTime[2] = digiDisplay.encodeDigit(timeClient.getMinutes() / 10);
      displayTime[3] = digiDisplay.encodeDigit(timeClient.getMinutes() % 10);
      if (isCLKPointON == false) {
        isCLKPointON = true;
      } else {
        displayTime[1] |= 128;    
        isCLKPointON = false;
      }
      digiDisplay.setSegments(displayTime, 4, 0);
  } else if (lastSysTimeCLK > millis()) { //After millis reset, lastSysTimeLED must be bigger.
      lastSysTimeCLK = millis();
  }
}

void buttonDetect() {
  if (digitalRead(14) == 1) {
    if (isChangedBTN == false) {
      isChangedBTN = true;
      lastSysTimeBTN = millis();
    } else {
      if (lastSysTimeBTN + 500 <= millis()) {
        if (RGBMode < 3) {
          RGBMode++;
        } else {
          RGBMode = 0;
        }
        ledRGBCount = 0;
        ledRGBState = 0;
        isChangedBTN = false;
        Serial.printf("[I] RGB Display mode changed to [%d]\n", RGBMode);
      }
    }
  } else {
    isChangedBTN = false;
  }
}


void ledDisplay() {
  if (RGBMode == 0){
    if (lastSysTimeLED + DELAY_MS <= millis()) {
      lastSysTimeLED = millis();
      ledRGBCount++;
    } else if (lastSysTimeLED > millis()) { //After millis reset, lastSysTimeLED must be bigger.
      lastSysTimeLED = millis();
    }
    if (ledRGBCount <= 255) {
      if (ledRGBState == 0) {
        leds[0] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[1] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[2] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[3] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[4] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[5] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[6] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[7] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[8] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[9] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[10] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[11] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        FastLED.show();      
      } else if (ledRGBState == 1) {
        leds[0] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[1] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[2] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[3] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[4] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[5] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[6] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[7] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[8] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[9] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[10] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[11] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        FastLED.show();
      } else if (ledRGBState == 2) {
        leds[0] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[1] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[2] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[3] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[4] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[5] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[6] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[7] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[8] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[9] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[10] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[11] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        FastLED.show();
      }
    } else {
      ledRGBCount = 0;
      if (ledRGBState < 2) {
        ledRGBState++;
      } else {
        ledRGBState = 0;
      }
    }
  } else if (RGBMode == 1) {
    if (lastSysTimeLED + DELAY_MS <= millis()) {
      lastSysTimeLED = millis();
      ledRGBCount++;
    } else if (lastSysTimeLED > millis()) { //After millis reset, lastSysTimeLED must be bigger.
      lastSysTimeLED = millis();
    }
    if (ledRGBCount <= 255) {
      if (ledRGBState == 0) {
        leds[0] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[1] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[2] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[3] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[4] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[5] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[6] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[7] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[8] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[9] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[10] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[11] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        FastLED.show();      
      } else if (ledRGBState == 1) {
        leds[0] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[1] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[2] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[3] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[4] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[5] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[6] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[7] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[8] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[9] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[10] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[11] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        FastLED.show();
      } else if (ledRGBState == 2) {
        leds[0] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[1] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[2] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[3] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[4] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[5] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[6] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[7] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[8] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[9] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[10] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        leds[11] = CRGB(ledRGBCount , 0, 255 - ledRGBCount);
        FastLED.show();
      }
    } else {
      ledRGBCount = 0;
      if (ledRGBState < 2) {
        ledRGBState++;
      } else {
        ledRGBState = 0;
      }
    }
  } else if (RGBMode == 2) {
    if (lastSysTimeLED + DELAY_MS <= millis()) {
      lastSysTimeLED = millis();
      ledRGBCount++;
    } else if (lastSysTimeLED > millis()) { //After millis reset, lastSysTimeLED must be bigger.
      lastSysTimeLED = millis();
    }
    if (ledRGBCount <= 255) {
      if (ledRGBState == 0) {
        leds[9] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[10] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        leds[11] = CRGB(255 - ledRGBCount, ledRGBCount, 0);
        
        if (ledRGBCount <= 215) {
          leds[0] = CRGB(215 - ledRGBCount, ledRGBCount + 40, 0);
          leds[8] = CRGB(215 - ledRGBCount, ledRGBCount + 40, 0);
        } else {
          leds[0] = CRGB(0, 470 - ledRGBCount, ledRGBCount - 215);
          leds[8] = CRGB(0, 470 - ledRGBCount, ledRGBCount - 215);
        }

        if (ledRGBCount <= 175) {
          leds[1] = CRGB(175 - ledRGBCount, ledRGBCount + 80, 0);
          leds[7] = CRGB(175 - ledRGBCount, ledRGBCount + 80, 0);
        } else {
          leds[1] = CRGB(0, 430 - ledRGBCount, ledRGBCount - 175);
          leds[7] = CRGB(0, 430 - ledRGBCount, ledRGBCount - 175);
        }

        if (ledRGBCount <= 135) {
          leds[2] = CRGB(135 - ledRGBCount, ledRGBCount + 120, 0);
          leds[6] = CRGB(135 - ledRGBCount, ledRGBCount + 120, 0);
        } else {
          leds[2] = CRGB(0, 390 - ledRGBCount, ledRGBCount - 135);
          leds[6] = CRGB(0, 390 - ledRGBCount, ledRGBCount - 135);
        }

        if (ledRGBCount <= 95) {
          leds[3] = CRGB(95 - ledRGBCount, ledRGBCount + 160, 0);
          leds[4] = CRGB(95 - ledRGBCount, ledRGBCount + 160, 0);
          leds[5] = CRGB(95 - ledRGBCount, ledRGBCount + 160, 0);
        } else {
          leds[3] = CRGB(0, 350 - ledRGBCount, ledRGBCount - 95);
          leds[4] = CRGB(0, 350 - ledRGBCount, ledRGBCount - 95);
          leds[5] = CRGB(0, 350 - ledRGBCount, ledRGBCount - 95);
        }

        FastLED.show();
      } else if (ledRGBState == 1) {
        leds[9] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[10] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        leds[11] = CRGB(0, 255 - ledRGBCount, ledRGBCount);
        
        if (ledRGBCount <= 215) {
          leds[0] = CRGB(0, 215 - ledRGBCount, ledRGBCount + 40);
          leds[8] = CRGB(0, 215 - ledRGBCount, ledRGBCount + 40);
        } else {
          leds[0] = CRGB(ledRGBCount - 215, 0, 470 - ledRGBCount);
          leds[8] = CRGB(ledRGBCount - 215, 0, 470 - ledRGBCount);
        }

        if (ledRGBCount <= 175) {
          leds[1] = CRGB(0, 175 - ledRGBCount, ledRGBCount + 80);
          leds[7] = CRGB(0, 175 - ledRGBCount, ledRGBCount + 80);
        } else {
          leds[1] = CRGB(ledRGBCount - 175, 0, 430 - ledRGBCount);
          leds[7] = CRGB(ledRGBCount - 175, 0, 430 - ledRGBCount);
        }

        if (ledRGBCount <= 135) {
          leds[2] = CRGB(0, 135 - ledRGBCount, ledRGBCount + 120);
          leds[6] = CRGB(0, 135 - ledRGBCount, ledRGBCount + 120);
        } else {
          leds[2] = CRGB(ledRGBCount - 135, 0, 390 - ledRGBCount);
          leds[6] = CRGB(ledRGBCount - 135, 0, 390 - ledRGBCount);
        }

        if (ledRGBCount <= 95) {
          leds[3] = CRGB(0, 95 - ledRGBCount, ledRGBCount + 160);
          leds[4] = CRGB(0, 95 - ledRGBCount, ledRGBCount + 160);
          leds[5] = CRGB(0, 95 - ledRGBCount, ledRGBCount + 160);
        } else {
          leds[3] = CRGB(ledRGBCount - 95, 0, 350 - ledRGBCount);
          leds[4] = CRGB(ledRGBCount - 95, 0, 350 - ledRGBCount);
          leds[5] = CRGB(ledRGBCount - 95, 0, 350 - ledRGBCount);
        }
        FastLED.show();
      } else if (ledRGBState == 2) {
        leds[9] = CRGB(ledRGBCount, 0, 255 - ledRGBCount);
        leds[10] = CRGB(ledRGBCount, 0, 255 - ledRGBCount);
        leds[11] = CRGB(ledRGBCount, 0, 255 - ledRGBCount);
        
        if (ledRGBCount <= 215) {
          leds[0] = CRGB(ledRGBCount + 40, 0, 215 - ledRGBCount);
          leds[8] = CRGB(ledRGBCount + 40, 0, 215 - ledRGBCount);
        } else {
          leds[0] = CRGB(470 - ledRGBCount, ledRGBCount - 215, 0);
          leds[8] = CRGB(470 - ledRGBCount, ledRGBCount - 215, 0);
        }

        if (ledRGBCount <= 175) {
          leds[1] = CRGB(ledRGBCount + 80 , 0, 175 - ledRGBCount);
          leds[7] = CRGB(ledRGBCount + 80 , 0, 175 - ledRGBCount);
        } else {
          leds[1] = CRGB(430 - ledRGBCount ,ledRGBCount - 175, 0);
          leds[7] = CRGB(430 - ledRGBCount ,ledRGBCount - 175, 0);
        }

        if (ledRGBCount <= 135) {
          leds[2] = CRGB(ledRGBCount + 120, 0, 135 - ledRGBCount);
          leds[6] = CRGB(ledRGBCount + 120, 0, 135 - ledRGBCount);
        } else {
          leds[2] = CRGB(390 - ledRGBCount ,ledRGBCount - 135, 0);
          leds[6] = CRGB(390 - ledRGBCount ,ledRGBCount - 135, 0);
        }

        if (ledRGBCount <= 95) {
          leds[3] = CRGB(ledRGBCount + 160, 0, 95 - ledRGBCount);
          leds[4] = CRGB(ledRGBCount + 160, 0, 95 - ledRGBCount);
          leds[5] = CRGB(ledRGBCount + 160, 0, 95 - ledRGBCount);
        } else {
          leds[3] = CRGB(350 - ledRGBCount, ledRGBCount - 95, 0);
          leds[4] = CRGB(350 - ledRGBCount, ledRGBCount - 95, 0);
          leds[5] = CRGB(350 - ledRGBCount, ledRGBCount - 95, 0);
        }
        FastLED.show();        
      }
    } else {
      ledRGBCount = 0;
      if (ledRGBState < 2) {
        ledRGBState++;
      } else {
        ledRGBState = 0;
      }
    }
  } else if (RGBMode == 3) {
    leds[0] = CRGB(0, 0, 0);
    leds[1] = CRGB(0, 0, 0);
    leds[2] = CRGB(0, 0, 0);
    leds[3] = CRGB(0, 0, 0);
    leds[4] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[7] = CRGB(0, 0, 0);
    leds[8] = CRGB(0, 0, 0);
    leds[9] = CRGB(0, 0, 0);
    leds[10] = CRGB(0, 0, 0);
    leds[11] = CRGB(0, 0, 0);
    FastLED.show();
  }
}
