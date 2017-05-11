/* 
 *  Created on 11. 5. 2017
 *  Author: Primož Hrovat
 *  
 */
#include "FastLED.h"
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

/***** DEFINED PINS *****/
#define TOUCH_SENSOR_1 2
#define TOUCH_SENSOR_2 3
#define TOUCH_SENSOR_3 4
#define TOUCH_SENSOR_4 5
#define RELAY_SWITCH_1 6
#define RELAY_SWITCH_2 7
#define RELAY_SWITCH_3 8
#define RELAY_SWITCH_4 9

#define NUM_LEDS 120
#define DATA_PIN 10

/***** CONSTANTS *****/
CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
uint8_t gHueDelta = 2;
int relay_on = 0;

/***** FUNCTION PROTOTYPES *****/
/***** SETUP *****/
void setupTouch();
void setupRelays();
void setupLED();
/***** LED ANIMATIONS *****/
void simple();
/***** PRACTICAL FUNCTIONS *****/
void setRelays(byte touch_sensor);
/***** INTERRUPT HANDLERS *****/
void switch_interrupt1();
void switch_interrupt2();
void switch_interrupt3();
void switch_interrupt4();

void setup() {

  // debuging
  Serial.begin(9600);

  setupTouch();
  setupRelays();
  setupLED();
}

void loop() {
  
  simple();
}

/***** SETUP *****/
void setupTouch() {
  // define input pins
  pinMode(TOUCH_SENSOR_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOUCH_SENSOR_1), switch_interrupt1, RISING);
  pinMode(TOUCH_SENSOR_2, INPUT);
  attachInterrupt(digitalPinToInterrupt(TOUCH_SENSOR_2), switch_interrupt2, RISING);
  pinMode(TOUCH_SENSOR_3, INPUT);
  PCintPort::attachInterrupt(TOUCH_SENSOR_3, switch_interrupt3, RISING);
  pinMode(TOUCH_SENSOR_4, INPUT);
  PCintPort::attachInterrupt(TOUCH_SENSOR_4, switch_interrupt4, RISING);
}

void setupRelays() {
  // define relay pins
  pinMode(RELAY_SWITCH_1, OUTPUT);
  pinMode(RELAY_SWITCH_2, OUTPUT);
  pinMode(RELAY_SWITCH_3, OUTPUT);
  pinMode(RELAY_SWITCH_4, OUTPUT); 
}

void setupLED() {
  // setup leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
}

/***** LED ANIMATIONS *****/

void simple() {
  // simple animation
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot].setHSV(gHue, 150, 150);
    FastLED.show();
    gHue += gHueDelta;
    delay(20);
  }
}

void setRelays(byte touch_sensor) {
  Serial.println(touch_sensor);
  switch (touch_sensor){
    case TOUCH_SENSOR_1:
      digitalWrite(RELAY_SWITCH_1, LOW);
      digitalWrite(RELAY_SWITCH_2, HIGH);
      digitalWrite(RELAY_SWITCH_3, HIGH);
      digitalWrite(RELAY_SWITCH_4, HIGH);
      break;
    case TOUCH_SENSOR_2:
      digitalWrite(RELAY_SWITCH_1, HIGH);
      digitalWrite(RELAY_SWITCH_2, LOW);
      digitalWrite(RELAY_SWITCH_3, HIGH);
      digitalWrite(RELAY_SWITCH_4, HIGH);
      break;
    case TOUCH_SENSOR_3:
      digitalWrite(RELAY_SWITCH_1, HIGH);
      digitalWrite(RELAY_SWITCH_2, HIGH);
      digitalWrite(RELAY_SWITCH_3, LOW);
      digitalWrite(RELAY_SWITCH_4, HIGH);
      break;
    case TOUCH_SENSOR_4:
      digitalWrite(RELAY_SWITCH_1, HIGH);
      digitalWrite(RELAY_SWITCH_2, HIGH);
      digitalWrite(RELAY_SWITCH_3, HIGH);
      digitalWrite(RELAY_SWITCH_4, LOW);
    default:
      digitalWrite(RELAY_SWITCH_1, HIGH);
      digitalWrite(RELAY_SWITCH_2, HIGH);
      digitalWrite(RELAY_SWITCH_3, HIGH);
      digitalWrite(RELAY_SWITCH_4, HIGH);
  }
}

// interrupt handlers

void switch_interrupt1() {
  if (relay_on != RELAY_SWITCH_1) {
    setRelays(TOUCH_SENSOR_1);
    relay_on = RELAY_SWITCH_1;
  }
  else { 
    setRelays(0);
    relay_on = 0;
  }
}

void switch_interrupt2() {
  if (relay_on != RELAY_SWITCH_2) {
    setRelays(TOUCH_SENSOR_2);
    relay_on = RELAY_SWITCH_2;
  }
  else { 
    setRelays(0);
    relay_on = 0;
  }
}

void switch_interrupt3() {
  if (relay_on != RELAY_SWITCH_3) {
    setRelays(TOUCH_SENSOR_3);
    relay_on = RELAY_SWITCH_3;
  }
  else { 
    setRelays(0);
    relay_on = 0;
  }
}

void switch_interrupt4() {
  if (relay_on != RELAY_SWITCH_4) {
    setRelays(TOUCH_SENSOR_4);
    relay_on = RELAY_SWITCH_4;
  }
  else { 
    setRelays(0);
    relay_on = 0;
  }  
}
