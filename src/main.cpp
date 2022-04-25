#include <Arduino.h>

const int R_PIN = 13;
const int G_PIN = 16;
const int B_PIN = 12;

const int R_INITIAL = 255;
const int G_INITIAL = 255;
const int B_INITIAL = 255;

const int FADE_IN = 1000;
const int FADE_DISABLED = 3000;
const int FADE_OUT = 1000;

const int INTERVAL = 50;

int rValue = 0;
int gValue = 0;
int bValue = 0;

void setLed(int r, int g, int b)
{
  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
  Serial.println("led set to r:" + String(r) + " g:" + String(g) + " b:" + String(b));
}

void fade(bool IN, int r, int g, int b)
{
  int steps = (IN ? FADE_IN : FADE_OUT) / INTERVAL;

  int rStep = r / steps;
  int gStep = g / steps;
  int bStep = b / steps;

  for (int i = 0; i < steps; i++)
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

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  setLed(rValue, gValue, bValue);
}

void loop()
{
  fade(true, R_INITIAL, G_INITIAL, B_INITIAL);
  delay(FADE_DISABLED);
  fade(false, rValue, gValue, bValue);
  Serial.println("LED is on");
  Serial.println(millis());
}