/*
Project IOT Terminal System
Based on ESP8266
Auther TurboHsu
SCL - D1
SDA - D2
*/
//TODO:菜单内查看的System Info。

//Include Modules
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <iostream>

//Config
const char *wifiSsid = "";
const char *wifiPassword = "";
const char *apiToken = "";
const int debugLevel = 1;
WiFiClient client;
ESP8266WebServer esp8266_server(80);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"ntp1.aliyun.com",60*60*8,30*60*1000);
int mode;
//MODE1 = CLOCK
//MODE2 = INFO

//Voids
void systemInit()
{
    Serial.begin(9600);
    while (!Serial)
        continue;

    Serial.println("[Info] IOT Terminal System INIT.");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("[Error] SSD1306 allocation failed"));
        for (;;)
            ;
    }

    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(14, INPUT);

    display.clearDisplay();
    display.display();
    displayTitle("System INIT");

    //Wifi Connection
    displayProcess(45);
    displayText(15, 20, 1, 1, "Establishing Wifi Connection....");
    display.display();
    Serial.print("[Info] Establishing Wifi connection...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSsid, wifiPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    //ESP8266WebServer
    esp8266_server.begin();
    esp8266_server.on("/", webRootHandler);
    esp8266_server.on("/api", webApiHandler);
    esp8266_server.onNotFound(web404Handler);
    Serial.println("[Info] ESP8266WebServer Started");

    //DataFetch
    displayClearX(15, 40);
    displayText(15, 20, 1, 1, "Fetching Data...");
    displayProcess(80);
    display.display();
    webdataFetch("1");

    //TODO:菜单内查看的System Info。

    Serial.println("[Info] Initialization done.");

    display.clearDisplay();
    display.display();
    //Default CLOCK
    mode = 1;
}

//Pages
void modeInfo()
{
    /*
    I', silly...
    I don't know how to connect them..
    char *dSsid;
    char *dIp;
    sprintf(dSsid,"SSID: %s",WiFi.localIP().toString().c_str());
    sprintf(dIp,"IP: %s",wifiSsid);
    */
    display.clearDisplay();
    displayTitle("System Info");
    displayText(0, 16, 1, 1, "TurboHsu's IOT System");
    displayText(0,26,1,1,"SSID:");
    displayText(35,26,1,1,wifiSsid);
    displayText(0,37,1,1,"IP:");
    displayText(35,37,1,1,WiFi.localIP().toString().c_str());
    displayText(20, 57, 1, 1, "Version:v0.0.3");
    display.display();
}

void modeClock(){
    timeClient.update();
    display.clearDisplay();
    displayText(20,20,1,2,timeClient.getFormattedTime().c_str());
    display.display();
}

void modeDetect()
{
    if (digitalRead(14) == 1)
    {
        delay(500);
        //Detect if pressed for 0.5s
        if (digitalRead(14) == 1)
        {
            if (mode <= 2){
                mode++;
            } else {
                mode = 1;
            }
        }
    }
    if (mode == 1)
    {
        modeClock();
    } else if (mode == 2)
    {
        modeInfo();
    }
}

//WebService
void webdataFetch(const char *m)
{
    if (m == "none")
    {
        //瞎bb
        Serial.println("[Info] Fetching none");
        delay(2500);
        displayClearX(15, 64);
        displayText(20, 20, 1, 1, "IP:");
        displayText(40, 20, 1, 1, WiFi.localIP().toString().c_str());
        display.display();
        delay(3000);
    } else if (m == "1")
    {
        timeClient.begin();
        Serial.println("[Info] Fetched Time");
        displayClearX(15, 64);
        displayProcess(95);
        displayText(20, 20, 1, 1, "Done");
        display.display();
        delay(500);
    }
}

//WebHandler

void webRootHandler()
{
    esp8266_server.send(200, "text/plain", "IOT Terminal System.");
}

void webApiHandler()
{
    //Token Auth
    String token = esp8266_server.arg("token");
    if (token == apiToken)
    {
        String apiMode = esp8266_server.arg("mode");
        //Modes
        if (apiMode == "display")
        {
            //Display Section
            String displayMode = esp8266_server.arg("dm");
            int16_t Clear = esp8266_server.arg("cls").toInt();
            int16_t Layout = esp8266_server.arg("out").toInt();
            if (Clear == 1)
            {
                display.clearDisplay();
            }
            if (displayMode == "text")
            {
                const char *Text = esp8266_server.arg("t").c_str();
                uint8_t Size = esp8266_server.arg("s").toInt();
                int16_t CursorX = esp8266_server.arg("x").toInt();
                int16_t CursorY = esp8266_server.arg("y").toInt();
                int16_t Color = esp8266_server.arg("c").toInt();
                displayText(CursorX, CursorY, Color, Size, Text);
            }
            else if (displayMode == "line")
            {
                int16_t a = esp8266_server.arg("a").toInt();
                int16_t b = esp8266_server.arg("b").toInt();
                int16_t x = esp8266_server.arg("x").toInt();
                int16_t y = esp8266_server.arg("y").toInt();
                uint16_t c = esp8266_server.arg("c").toInt();
                displayDrawLine(a, b, x, y, c);
            }
            if (Layout == 1)
            {
                display.display();
            }
        }
        else if (apiMode == "button")
        {
            //button
        }
        else
        {
            esp8266_server.send(500, "text/plain", "Invalid mode selection.");
        }
    }
    else
    {
        esp8266_server.send(403, "text/plain", "403 Forbidden.");
    }
}

void web404Handler()
{
    esp8266_server.send(404, "text/plain", "404 Not Found");
}

//Display Voids
void displayDrawLine(int16_t x, int16_t y, int16_t z, int16_t l, uint16_t c)
{
    display.drawLine(x, y, z, l, c);
}

void displayText(int16_t x, int16_t y, int16_t c, uint8_t s, const char *t)
{
    if (c == 1)
    {
        display.setTextColor(WHITE);
    }
    else
    {
        display.setTextColor(BLACK);
    }
    display.setTextSize(s);
    display.setCursor(x, y);
    display.println(t);
}

void displayTitle(const char *t)
{
    displayText(10, 0, 1, 1, t);
    for (int i = 0; i < 2; i++)
    {
        displayDrawLine(2, 7 + i, 126, 7 + i, 1);
    }
}

void displayProcess(int16_t p)
{
    //p 0-95
    //Draw a frame
    displayDrawLine(15, 41, 114, 41, 1);
    displayDrawLine(15, 41, 15, 50, 1);
    displayDrawLine(114, 41, 114, 50, 1);
    displayDrawLine(15, 50, 114, 50, 1);
    //Draw process
    for (int i = 0; i < 6; i++)
    {
        displayDrawLine(17, 43 + i, 17 + p, 43 + i, 1);
    }
}

void displayClearX(int16_t a, int16_t b)
{
    //Clear lines
    for (int i = a; i < b + 1; i++)
    {
        displayDrawLine(0, i, 128, i, 0);
    }
}



//Main void
void setup()
{
    systemInit();
}

void loop()
{
    modeDetect();
    esp8266_server.handleClient();
}
