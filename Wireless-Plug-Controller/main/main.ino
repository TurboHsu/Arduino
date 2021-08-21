/*
Project Name: Wireless Plug Controller
Author: TurboHsu
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

const char *wifiSsid = "";
const char *wifiPassword = "";
bool switchStatus;
WiFiClient client;
ESP8266WebServer esp8266_server(80);
IPAddress local_IP(192, 168, 10, 220);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 10, 1);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(14, OUTPUT);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS))
    {
    digitalWrite(LED_BUILTIN, LOW);
    }
    WiFi.begin(wifiSsid, wifiPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
    }
    digitalWrite(LED_BUILTIN, HIGH);

    esp8266_server.begin();
    esp8266_server.on("/", webRootHandler);
    esp8266_server.on("/api", webApiHandler);
    esp8266_server.onNotFound(web404Handler);
}

void loop() {
    esp8266_server.handleClient();
    switchDo();
}

void webRootHandler() {
    digitalWrite(LED_BUILTIN, LOW);
    esp8266_server.send(200, "text/html", "<!DOCTYPE html>\n<html>\n\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Smart Switch</title>\n\n    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/mdui@1.0.1/dist/css/mdui.min.css\" integrity=\"sha384-cLRrMq39HOZdvE0j6yBojO4+1PrHfB7a9l5qLcmRm/fiWXYY+CndJPmyu5FV/9Tw\" crossorigin=\"anonymous\" />\n</head>\n\n<body class=\"mdui-drawer-body-left mdui-appbar-with-toolbar mdui-theme-primary-indigo mdui-theme-accent-pink mdui-theme-layout-auto mdui-loaded\">\n    <!--Header-->\n    <header class=\"mdui-appbar mdui-appbar-fixed\">\n        <div class=\"mdui-toolbar mdui-color-theme\">\n            <a href=\".\" class=\"mdui-typo-headline\">Smart Switch</a>\n        </div>\n        <div class=\"mdui-toolbar-spacer\"></div>\n    </header>\n\n    <!--Drawer-->\n\n    <div class=\"mdui-container doc-container\">\n        <br>\n        <br>\n        <p>\n            <div class=\"mdui-card\">\n                <div class=\"mdui-card-primary\">\n                    <div class=\"mdui-card-primary-title\">Controller</div>\n                    <div class=\"mdui-card-primary-subtitle\">Contorl the switch</div>\n                </div>\n                <div class=\"mdui-card-menu\">\n                    <button class=\"mdui-btn mdui-btn-raised mdui-color-theme-100\">STATUS: <i class=\"mdui-icon material-icons\" id=\"statusDisplay\">radio_button_checked</i><b></b></button>\n\n                    <button class=\"mdui-btn mdui-btn-raised mdui-color-theme-accent\" onclick=\"doSwitch()\"><i class=\"mdui-icon material-icons\">power_settings_new</i>SWITCH</button>\n                </div>\n            </div>\n        </p>\n\n    </div>\n\n\n\n    <script src=\"https://cdn.jsdelivr.net/npm/mdui@1.0.1/dist/js/mdui.min.js\" integrity=\"sha384-gCMZcshYKOGRX9r6wbDrvF+TcCCswSHFucUzUPwka+Gr+uHgjlYvkABr95TCOz3A\" crossorigin=\"anonymous\"></script>\n    <script>\n        var $ = mdui.$;\n        window.onload = function() {\n            updateStatus();\n        }\n\n        function doSwitch() {\n            $.ajax({\n                method: 'POST',\n                url: './api',\n                data: {\n                    do: \"switch\"\n                },\n                async: false,\n                success: function(data) {\n                    if (data == \"ok\") {\n                        mdui.snackbar({\n                            message: 'Switch success.'\n                        });\n                        updateStatus();\n                    } else {\n                        mdui.snackbar({\n                            message: 'Error.'\n                        });\n                        updateStatus();\n                    }\n                }\n            })\n        }\n\n        function updateStatus() {\n            $.ajax({\n                method: 'POST',\n                url: './api',\n                data: {\n                    do: \"check\"\n                },\n                async: false,\n                success: function(data) {\n                    if (data == \"on\") {\n                        document.getElementById('statusDisplay').style.color = 'mediumspringgreen';\n                    } else if (data == \"off\") {\n                        document.getElementById('statusDisplay').style.color = 'red';\n                    } else {\n                        document.getElementById('statusDisplay').style.color = 'grey';\n                    }\n                }\n            })\n        }\n    </script>\n</body>\n\n</html>");
    digitalWrite(LED_BUILTIN, HIGH);
}

void webApiHandler() {
    digitalWrite(LED_BUILTIN, LOW);
    String doText = esp8266_server.arg("do");
    if (doText == "check") {
        if (switchStatus == false) {
            esp8266_server.send(200, "text/plain", "off");
        } else {
            esp8266_server.send(200, "text/plain", "on");
        }
    } else if (doText == "switch") {
        if (switchStatus == false) {
            switchStatus = true;
            esp8266_server.send(200, "text/plain", "ok");
        } else {
            switchStatus = false;
            esp8266_server.send(200, "text/plain", "ok");
        }
    } else {
        esp8266_server.send(502, "text/plain", "Bad request.");
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

void web404Handler() {
    digitalWrite(LED_BUILTIN, LOW);
    esp8266_server.send(404, "text/plain", "404 Not Found.");
    digitalWrite(LED_BUILTIN, HIGH);
}

void switchDo() {
    if (switchStatus == false) {
        digitalWrite(14, LOW);
    } else {
        digitalWrite(14, HIGH);
    }
}