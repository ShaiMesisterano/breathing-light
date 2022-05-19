#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "BreathingLight"
#endif

const char *ssid = APSSID;

ESP8266WebServer server(80);

const int R_PIN = 13;
const int G_PIN = 16;
const int B_PIN = 12;

const int R_NONE = 0;
const int G_NONE = 0;
const int B_NONE = 0;

int fadeIn = 1;
int fadeDisabled = 3;
int fadeOut = 1;

const int INTERVAL = 100;

int rConfiguration = R_NONE;
int gConfiguration = G_NONE;
int bConfiguration = B_NONE;

int rValue = rConfiguration;
int gValue = gConfiguration;
int bValue = bConfiguration;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>Breathing Light | Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <form action="/save">
      <h3>Breathing Light - Configuration</h3>
      <p>
        <label for="color-r">Color</label>
        <!-- <input type="color" name="color" onchange="convertToRgb()"> -->
        <!-- COLORS -->
        R: <input type="number" name="color-r" value="0">
        G: <input type="number" name="color-g" value="0">
        B: <input type="number" name="color-b" value="0">
      </p>

      <p>
        <label for="inhale">Inhale</label>
        <input type="number" name="inhale" value="0"> Seconds
      </p>

      <p>
        <label for="break">Break</label>
        <input type="number" name="break" value="0"> Seconds
      </p>

      <p>
        <label for="exhale">Exhale</label>
        <input type="number" name="exhale" value="0"> Seconds
      </p>

      <input type="submit" value="Save">
    </form>
    <style>
      input[type="number"] {
        width: 50px;
      }
    </style>
    <script>
      function convertToRgb(){
        const color = document.getElementsByName('color')[0].value;
        document.getElementsByName('color-r')[0].value = color.slice(1,3);
        document.getElementsByName('color-g')[0].value = color.slice(3,5);
        document.getElementsByName('color-b')[0].value = color.slice(5,7);
      }
    </script>
  </body>
</html>)rawliteral";

const char success_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>Breathing Light | Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <h5>Saved succussfully!</h5><a href='/'>Back</a>
  </body>
</html>)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}


void handleSave() {
  rConfiguration = server.arg("color-r").toInt();
  gConfiguration = server.arg("color-g").toInt();
  bConfiguration = server.arg("color-b").toInt();
  fadeIn = server.arg("inhale").toInt();
  fadeDisabled = server.arg("break").toInt();
  fadeOut = server.arg("exhale").toInt();

  Serial.println("saved configuration: r: " + String(rConfiguration) + " g:" + String(gConfiguration) + " b:" + String(bConfiguration) + " fade in:" + String(fadeIn) + " fade out:" + String(fadeIn) + " disabled:" + String(fadeDisabled));

  server.send(200, "text/html", success_html);
}

void setLed(int r, int g, int b)
{
  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
}

void fade(bool IN, int rTarget, int gTarget, int bTarget)
{
  float steps = ((IN ? fadeIn : fadeOut) * 1000) / INTERVAL;

  Serial.println("steps:" + String(steps));

  float rStep = rConfiguration / steps;
  float gStep = gConfiguration / steps;
  float bStep = bConfiguration / steps;

  Serial.println("color steps: r: " + String(rStep) + " g:" + String(gStep) + " b:" + String(bStep));

  for (float i = 0; i < steps; i++)
  {
    if (IN) {
      rValue += rStep;
      gValue += gStep;
      bValue += bStep;
    } else {
      rValue -= rStep;
      gValue -= gStep;
      bValue -= bStep;
    }
    
    setLed(rValue, gValue, bValue);
    delay(INTERVAL);
  }
}

void setup()
{
  Serial.begin(115200);

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on ("/save", handleSave);
  server.begin();
  Serial.println("HTTP server started");

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  setLed(rConfiguration, gConfiguration, bConfiguration);
}

void loop()
{
  server.handleClient();

  Serial.println("---" + String(millis() / 1000) + " fading in: r: " + String(rConfiguration) + " g:" + String(gConfiguration) + " b:" + String(bConfiguration));
  fade(true, rConfiguration, gConfiguration, bConfiguration);

  Serial.println("---" + String(millis() / 1000) + " pausing");
  delay(fadeDisabled * 1000);

  Serial.println("---" + String(millis() / 1000) + " fading out: r: " + String(R_NONE) + " g:" + String(G_NONE) + " b:" + String(B_NONE));
  fade(false, R_NONE, G_NONE, B_NONE);

  Serial.println("---" + String(millis() / 1000) + " pausing");
  delay(fadeDisabled * 1000);
}