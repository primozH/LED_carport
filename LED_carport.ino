/* 
 *  Created on 11. 5. 2017
 *  Author: Primož Hrovat
 *  
 */
#include <FastLED.h>
#include <PinChangeInt.h>

/***** DEFINED PINS *****/
#define TOUCH_SENSOR_1 2
#define TOUCH_SENSOR_2 3
#define TOUCH_SENSOR_3 4
#define TOUCH_SENSOR_4 5
#define RELAY_SWITCH_1 6
#define RELAY_SWITCH_2 7

#define TRIGPIN 8
#define ECHOPIN 9

#define NUM_LEDS 88
#define DATA_PIN 10

/***** OPERATION MODE *****/
#define AUTO_MODE 100
#define ANIMATION_MODE 101
#define AMBILIGHT_MODE 102

/***** CONSTANTS *****/
CRGBArray<NUM_LEDS> leds;
uint8_t gHue = 0;
uint8_t gHueDelta = (uint8_t) round(255 / NUM_LEDS);
uint8_t global = 0;
int relay_on = 0;
byte omode = 0;

/***** FUNCTION PROTOTYPES *****/
/***** SETUP *****/
void setupTouch();
void setupRelays();
void setupLED();
void setupDistSensor();
/***** LED ANIMATIONS *****/
void bounce();
void avadaKedavra();
void rainbow();
/***** PRACTICAL FUNCTIONS *****/
void setRelays(byte touch_sensor);
/***** INTERRUPT HANDLERS *****/
void switch_interrupt1();
void switch_interrupt2();
void switch_interrupt3();
void switch_interrupt4();

/***** MAIN PROGRAM *****/
void setup() {

  // debuging
  Serial.begin(9600);

  setupTouch();
  setupRelays();
  setupDistSensor();
  setupLED();

  ambimode();
}

void loop() {
  
}

/***** FUNCTION DEFINITIONS *****/
/***** SETUP *****/
void setupTouch() {
  // define input pins
  pinMode(TOUCH_SENSOR_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOUCH_SENSOR_1), switch_interrupt1, RISING);
  pinMode(TOUCH_SENSOR_2, INPUT);
  attachInterrupt(digitalPinToInterrupt(TOUCH_SENSOR_2), switch_interrupt2, RISING);
  pinMode(TOUCH_SENSOR_3, INPUT);
  PCintPort::attachInterrupt(TOUCH_SENSOR_3, switch_interrupt3, RISING);
}

void setupRelays() {
  // define relay pins
  pinMode(RELAY_SWITCH_1, OUTPUT);
  pinMode(RELAY_SWITCH_2, OUTPUT); 
}

void setupLED() {
  // setup leds
  delay(3000);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
}

void setupDistSensor() {
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
}

/***** LED ANIMATIONS *****/

void bounce(int numLeds, int del) {
  for (int dot = 0; dot < NUM_LEDS - numLeds + 1; dot++) {
    leds(dot, dot + numLeds - 1) = CHSV(gHue, 200, 150);
    FastLED.show();
    leds(dot, dot + numLeds - 1) = CHSV(gHue, 200, 0);
    gHue += gHueDelta;
    delay(del/(dot+1));
  }
  // bounce back
  for (int i = 1; i < NUM_LEDS-numLeds+1; i++) {
    int dot = NUM_LEDS - i;
    leds(dot - numLeds, dot - 1) = CHSV(gHue, 200, 150);
    FastLED.show();
    leds(dot - numLeds, dot - 1) = CHSV(gHue, 200, 0);
    gHue -= gHueDelta;
    delay(del/(dot));
  }
}

void rainbow(int del) {
  static uint8_t hue = 0;
  for (int i = 0; i < del; i++) {
    leds(0,NUM_LEDS/2 - 1).fill_rainbow(hue++); 
    leds(NUM_LEDS/2, NUM_LEDS-1) = leds(NUM_LEDS/2-1,0);
    FastLED.delay(del / 2);
  }
}

/* Voldemort vs. Harry
 *  args: nRounds = length of animation
 *  volC: Voldemort's color
 *  harryC: Harry's color
 *  winC1: Voldemort's win color
 *  winC2: Harry's win color
 *  del: speed of animation
 *  pause: 
 */
void avadaKedavra(int nRounds, uint8_t volC, uint8_t harryC, uint8_t winC1, uint8_t winC2, int del, int pause) {;

  for (int dot = 0; dot < NUM_LEDS/2; dot++) {
    leds[dot].setHSV(volC, 255, 150);
    leds[NUM_LEDS-dot - 1].setHSV(harryC, 255, 150);
    FastLED.show();
    FastLED.delay(del);
  }
  
  int half = NUM_LEDS / 2;
  
  for (int i = 0; i < nRounds; i++) {
    for (int a = half - 5; a <= half + 4; a++) {
      leds[a] = CHSV( random8(),200,100);
    }
    FastLED.show();
    if (random8() > 125){
      leds[half + 4] = CHSV(harryC, 255, 150);
      half--;
    }
    else {
      leds[half - 5] = CHSV(volC, 255, 150);
      half++;
    }
    FastLED.delay(del);
  }

// Harry WINS
  if (half < NUM_LEDS / 2) {
    while (half > 4) {    
      for (int i = half - 5; i <= half + 4; i++) {
        leds[i] = CHSV(random8(), 255, 150);
      }
      leds[half + 4] = CHSV(harryC, 255, 150);
      FastLED.show();
      half--;
      FastLED.delay(del);
    }

    for (int i = 9; i >= 0; i--) {
      leds[i] = CHSV(harryC, 255, 150);
      FastLED.show();
      FastLED.delay(del);
    }

    leds.fadeToBlackBy(255);
    FastLED.delay(pause);
    
    for (int i = 4; i < NUM_LEDS - 5; i++) {
      leds(i - 4, i + 5) = CHSV(winC1, 255, 50);
      FastLED.show();
      FastLED.delay(del);
    }
    
// Harry LOSES        
  } else {
    while (half < NUM_LEDS - 4) {
      for (int a = half - 5; a <= half + 4; a++)
        leds[a] = CHSV(random8(), 255, 100);
        
      leds[half - 5] = CHSV(volC, 255, 150);
      FastLED.show();
      half++;
      FastLED.delay(del);
    }

    for (int i = NUM_LEDS - 10; i < NUM_LEDS; i++) {
      leds[i] = CHSV(volC, 255, 150);
      FastLED.show();
      FastLED.delay(del);
    }

    leds.fadeToBlackBy(255);
    FastLED.delay(pause);
    
    for (int i = NUM_LEDS - 1; i >= 4; i--) {
      leds(i - 4, i + 5) = CHSV(winC2, 255, 50);
      FastLED.show();
      FastLED.delay(del);
    }
  }
/*  
  for (uint8_t sat = 20; sat < 255; sat++) {
      leds(half-5, half+4) = CHSV(40, sat, 150);
      FastLED.show();   
      delay(10);
  }
*/
  FastLED.delay(pause * 2);
  leds.fadeToBlackBy(255);
  FastLED.delay(pause * 4);
}


/***** RELAY ON *****/
void setRelays(byte mode) {
  Serial.println(mode);
  switch (mode){
    // all relays OFF //
    case 0:
      digitalWrite(RELAY_SWITCH_1, HIGH);
      digitalWrite(RELAY_SWITCH_2, HIGH);
      break;
    // Relay 1 & 2 ON
    case 1:
      digitalWrite(RELAY_SWITCH_1, LOW);
      digitalWrite(RELAY_SWITCH_2, LOW);
      break;
  }
}

// reads and return distance
int measureDistance() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  int pulseT = pulseIn(ECHOPIN, HIGH, 38000);
  if (pulseT == 0)
    return -1;
  int distance = pulseT/58;
  return distance;
}

/***** OPERATION MODES *****/
void automode() {
  delay(100);
  omode = AUTO_MODE;
  while(true) {
    delay(5000);
    int dist = measureDistance();
    if (dist > 150) {
      for (int i = 0; i < 10; i++) {
        bounce(10, 1000);
        delay(500);
      }
      // white leds for 5 minutes
      leds(0, NUM_LEDS -1) = CRGB::White;
      FastLED.show();
      delay(300000);
      leds.fadeToBlackBy(40);
    }
    
  }
}

void animationmode() {
  delay(100);
  omode = ANIMATION_MODE;
  while(true) {
    bounce(5, 200);
    delay(400);
  }
}

void ambimode() {
  delay(100);
  omode = AMBILIGHT_MODE;
  while(true) {
    byte color = random8();
    leds(0, NUM_LEDS-1) = CHSV(color, 255, 150);
    FastLED.show();
    delay(120000);
  }
}

/***** INTERRUPTS HANDLERS *****/
void switch_interrupt1() {
  if (omode != AUTO_MODE) {
    setRelays(1);
    automode();
  }
  else { 
    setRelays(0);
    omode = 0;
  }
}

void switch_interrupt2() {
  if (omode != ANIMATION_MODE) {
    setRelays(1);
    animationmode();
  }
  else { 
    setRelays(0);
    omode = 0;
  }
}

void switch_interrupt3() {
  if (omode != AMBILIGHT_MODE) {
    setRelays(1);
    ambimode();
  }
  else { 
    setRelays(0);
    omode = 0;
  }
}
