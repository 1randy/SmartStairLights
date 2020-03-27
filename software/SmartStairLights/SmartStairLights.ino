#include <FastLED.h>

#define VERSION 2020032701

// define sendor input pins
#define INPUT_PIN_1     A0      // 18 A0
#define INPUT_PIN_2     A1      // 19 A1


// Setup LED connection
#define DATA_PIN        2
#define CLOCK_PIN       3
#define NUM_LEDS        6       // count of all LED ports
#define CHIPSET         WS2801
#define COLOR_ORDER     RGB

#define BRIGHTNESS_MIN  10      // dim level for inactivity
#define BRIGHTNESS_MAX  255     // max brightness of the LED Stripes

#define WAIT            1000    // wait time after a sequence in ms
#define WAIT_PIN_READ   250     // time between input status query in ms
#define WAIT_PIN_MAXW   100     // timeout of second timer event in ms = WAIT_PIN_READ * WAIT_PIN_MAXW(ait)

#define FADE_WAIT       2       // speed of LED dimming
#define FADE_STEP       20      // steps of LED brightness levels

CRGB leds[NUM_LEDS];

void setup() {
    delay( 3000 ); // power-up safety delay

    pinMode(INPUT_PIN_1, INPUT);           // set pin to input
    pinMode(INPUT_PIN_2, INPUT);           // set pin to input
    digitalWrite(INPUT_PIN_1, HIGH);       // turn on pullup resistors
    digitalWrite(INPUT_PIN_2, HIGH);       // turn on pullup resistors

    FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

    // LED check
    FadeSequenceForward(1); 
    FadeSequenceBackward(0);

    Serial.println("setup done.");
}

void FadeUp(int lednum, int color) {
  for ( int brightness = BRIGHTNESS_MIN; brightness < BRIGHTNESS_MAX; brightness=brightness+FADE_STEP) {
    leds[lednum][color] = brightness;
    FastLED.show();
    delay(FADE_WAIT + 2);
  } 
  leds[lednum][color] = BRIGHTNESS_MAX;
  FastLED.show();
}


void FadeDown(int lednum, int color) {
  for ( int brightness = BRIGHTNESS_MAX; brightness > BRIGHTNESS_MIN; brightness=brightness-FADE_STEP ) {
    leds[lednum][color] = brightness;
    FastLED.show();
    delay(FADE_WAIT + 2);
  } 
  leds[lednum][color] = BRIGHTNESS_MIN;
  FastLED.show();
}

void FadeSequenceForward(bool led_on) {
  Serial.println("FadeSequenceForward()");
  for ( int out = 0; out < NUM_LEDS-1; out++ ) {
    for ( int color = 0; color < 3; color++ ) {
      if (led_on) {
        FadeUp(out, color);
      }  else {
        FadeDown(out,color);
      }
    }
  }
  delay(WAIT);
}

void FadeSequenceBackward(bool led_on) {
  Serial.println("FadeSequenceBackward()");
  for ( int out = NUM_LEDS-1; out >= 0; out-- ) {
    for ( int color = 2; color >= 0; color-- ) {
      if (led_on) {
        FadeUp(out, color);
      }  else {
        FadeDown(out,color);
      }
    }
  }
  delay(WAIT);
}

void loop()
{
  uint16_t wait_timout = 0;

  // forward sequence 1 -> MAX_LED
  if (!digitalRead(INPUT_PIN_1)) {
    Serial.println("start sequence on event1");
    FadeSequenceForward(1);
    while(digitalRead(INPUT_PIN_2) && wait_timout < WAIT_PIN_MAXW) {
      if (!digitalRead(INPUT_PIN_1)) {
        wait_timout = 0;
      } else {
        wait_timout++;
      }
      delay(WAIT_PIN_READ);
    }
    Serial.println("stop sequence on event2");
    FadeSequenceForward(0);
  }
  
  // backward sequence MAX_LED -> 1
  if (!digitalRead(INPUT_PIN_2)) {
    Serial.println("start sequence on event2");
    FadeSequenceBackward(1);
    while(digitalRead(INPUT_PIN_1) && wait_timout < WAIT_PIN_MAXW) {
      if (!digitalRead(INPUT_PIN_2)) {
        wait_timout = 0;
      } else {
        wait_timout++;
      }
      delay(WAIT_PIN_READ);
    }
    Serial.println("stop sequence on event1");
    FadeSequenceBackward(0);
  }

} // END main loop
