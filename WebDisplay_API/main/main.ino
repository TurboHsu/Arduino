/*
 * Project : ESP8266 Display API Server
 * Auther : TurboHsu
 * Version 0.0.3
 */

//Include Modules
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Config
const char *ssid = "";
const char *password = "";
const char *token = "";

//Initialization
WiFiClient client;
ESP8266WebServer esp8266_server(80);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    continue;
  //-------------Init--------------

  Serial.println("[Info] ESP8266 Display API Server - Ver0.1 - Powered by TurboCloud IOT");

  //SSD1306 Display Test

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("[Error] SSD1306 allocation failed"));
    for (;;)
      ;
  }

  //Wifi Connection Init

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Establishing WiFi Connection...");
  display.display();
  Serial.print("[Info] Connecting WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int connectCount = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    connectCount = connectCount + 10;
    display.setCursor(connectCount, 30);
    display.println(".");
    display.display();
  }
  Serial.println("");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Wifi connected.");
  display.setCursor(0, 25);
  display.println("IP address: ");
  display.setCursor(0, 40);
  display.println(WiFi.localIP());
  display.display();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1500);

  //WebServer Init
  esp8266_server.begin();
  esp8266_server.on("/", handleRoot);
  esp8266_server.on("/api", handleApi);
  esp8266_server.onNotFound(handleNotFound);
  Serial.println("[Info] WebServer Started.");
  //-------------Init--------------

  //-------------Display--------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("TurboCloud IOT Backend");
  display.setCursor(0, 25);
  display.println("Version: 0.0.3");
  display.display();
}

void loop()
{
  //WebServer Loop
  esp8266_server.handleClient();
}

//------------void----------------
//Webserver Handler
void handleRoot()
{
  esp8266_server.send(200, "text/plain", "TurboCloud IOT Backend. ESP8266 Display API v0.0.3");
}

void handleNotFound()
{
  esp8266_server.send(404, "text/plain", "404: Not found");
}

void handleApi()
{
  int displayClear = esp8266_server.arg("clear").toInt();
  String displayText = esp8266_server.arg("text");
  int displaySize = esp8266_server.arg("size").toInt();
  int displayCursorX = esp8266_server.arg("cx").toInt();
  int displayCursorY = esp8266_server.arg("cy").toInt();
  String apiToken = esp8266_server.arg("token");
  if (apiToken == token)
  {
    if (displayClear == 1)
    {
      display.clearDisplay();
    }
    display.setTextSize(displaySize);
    display.setCursor(displayCursorX, displayCursorY);
    display.println(displayText);
    display.display();
    esp8266_server.send(200, "text/plain", "Displayed.");
    Serial.println("[Info] API Receved request.");
  }
  else
  {
    Serial.println("[Info] API is accessed with an invalid token.");
    esp8266_server.send(403, "text/plain", "Token Wrong!");
  }
}
