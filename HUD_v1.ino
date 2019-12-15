#include <OBD2UART.h>
#include <Adafruit_NeoPixel.h>
#include <elapsedMillis.h>
#include "TimerOne.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

// For Teensy
#define OBDUART Serial1

//#define TEST

#define DIGIT_1_PIN 13
#define DIGIT_2_PIN 2
#define DIGIT_3_PIN 3
#define DIGIT_4_PIN 4

#define A_PIN 5
#define B_PIN 6
#define C_PIN 7
#define D_PIN 8
#define E_PIN 9
#define F_PIN 10
#define G_PIN 11
#define DP_PIN 12


#define LED_STRIP1_PIN 14
#define LED_STRIP2_PIN 15

#define BTN_BRIGHTER 16
#define BTN_DIMMER 17

//#define DIGIT_1_PIN 6
//#define DIGIT_2_PIN 8
//#define DIGIT_3_PIN 9
//#define DIGIT_4_PIN 12
//
//#define A_PIN A2
//#define B_PIN 4
//#define C_PIN 7
//#define D_PIN 11
//#define E_PIN 10
//#define F_PIN A1
//#define G_PIN 5
//#define DP_PIN A3

// #define LED_STRIP1_PIN A4
// #define LED_STRIP2_PIN A5

  
// //#define LED_PIN 9
// #define BTN_BRIGHTER 2
// #define BTN_DIMMER 3

int maxThrottle = 80; //100;
int maxRPM = 7000;
int max7SegmentPeriod = 4000;

volatile int speedVar = 8888;
int throttleVar = maxThrottle;
int rpmVar = maxRPM;
int dispPeriod = max7SegmentPeriod;

float brightness = 0.1;
bool waitingForData = false;
int failCount = 0;
int lastDigiDelayUs = 500;
bool displaySet = false;

int updateIntervalUs = 10000; //400;
int digitsToDisplay[3];
int digitCount = 0;
int currDigitIdx = 0;

enum dataTypes {
  TYPE_SPEED,
  TYPE_THROTTLE,
  TYPE_RPM
} toReadNext = TYPE_SPEED;


struct color_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
  color_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    red = r;
    green = g;
    blue = b;
    alpha = a;
  }
};

color_t color_rpm_low(255,128,64,255);
color_t color_rpm_mid(255,128,0,255);
color_t color_rpm_high(255,0,0,255);
color_t color_off(0,0,0,0);

COBD obd;
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(15, LED_STRIP1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(15, LED_STRIP2_PIN, NEO_GRB + NEO_KHZ800);

IntervalTimer timer1;



void setup(){
  
// #ifdef TEST
  Serial.begin(115200);
// #endif

  pinMode(DIGIT_1_PIN, OUTPUT);
  pinMode(DIGIT_2_PIN, OUTPUT);
  pinMode(DIGIT_3_PIN, OUTPUT);
  pinMode(DIGIT_4_PIN, OUTPUT);

  pinMode(A_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  pinMode(C_PIN, OUTPUT);
  pinMode(D_PIN, OUTPUT);
  pinMode(E_PIN, OUTPUT);
  pinMode(F_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(DP_PIN, OUTPUT);
  
  pinMode(LED_STRIP1_PIN, OUTPUT);
  pinMode(LED_STRIP2_PIN, OUTPUT);
  
  for(int i=4000; i>0; i--){
    displayDigit(0,0,false);
    displayDigit(0,1,false);
    displayDigit(0,2,false);
    displayDigit(0,3,false);
  }
  resetDisplay();

 Serial.println("beginning...");

#ifndef TEST
  obd.begin();
#endif

  strip1.begin();
  strip2.begin();

  strip1.show();
  strip2.show();

 Serial.println("initializing...");
 
#ifndef TEST
  while (!obd.init());
#endif


 Serial.println("Done");
  pinMode(BTN_BRIGHTER, INPUT_PULLUP);
  pinMode(BTN_DIMMER, INPUT_PULLUP);
  delay(500);
  attachInterrupt(digitalPinToInterrupt(BTN_BRIGHTER), incBrightness, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DIMMER), decBrightness, FALLING);
  
  timer1.begin(updateDisplay, updateIntervalUs);
}

void loop() {
  
// #ifdef TEST
//    int temp = getSpeedSerial();
// #else
//    int temp = getSpeedOBD() / 1.60934;
// #endif
// //
//    if(temp >= 0){
//      speedVar = temp;
//    }


    // int timeSinceInterrupt = Timer1.read();
    // Timer1.detachInterrupt();
    //delay(5);
    updateOBDValues();
    // Timer1.attachInterrupt(updateDisplay, dispPeriod-timeSinceInterrupt);
    
    setThrottle(throttleVar);
    setRPM(rpmVar);    

//    int toDelay = (float)100/brightness;
//    if(toDelay >= 1000) delay(toDelay/1000);
//    else delayMicroseconds(toDelay);
    
}

void getDigitsToDisplay(int speed, int* digitArr){

//  Serial.print(digits); Serial.print(" digits: ");

  //digits.clear();
  if(speed == 0){
    //displayDigit(0,0,false);
    digitArr[0] = 0;
    digitCount = 1;
  }
  else{
//    int digits = log10(speed) + 1;
    int digits = speed > 9 ? 2 : 1; 
    digitCount = 0;
    for(int i=0; i<digits; i++){
      int digit = (int)(speed/(pow(10,i)) ) % 10;
      // displayDigit(digit, i, false);
      // delayMicroseconds(digitTimeUs);
      digitArr[i] = digit;
      digitCount++;
    }
  }
}

void setThrottle(int throttle){
  for(int i=0; i<strip1.numPixels(); i++){
     if(i > ((float)throttle/maxThrottle)*strip1.numPixels()){
        strip1.setPixelColor(i, applyBrightness(color_off));
     }
     else {
        int green = 255*((float)i/strip1.numPixels());
        int blue = max(green-127, 0);
        color_t col(255, green, blue, 255);
        strip1.setPixelColor(i, applyBrightness(col));
     }
  }
  strip1.show();
}


void setRPM(int rpm){
  for(int i=0; i<strip2.numPixels(); i++){
     if(i > ((float)rpm/maxRPM)*strip2.numPixels()){
        strip2.setPixelColor(i, applyBrightness(color_off));
     }
     else if(i > 10){
        strip2.setPixelColor(i, applyBrightness(color_rpm_high));
     }
     else if(i > 6){
        strip2.setPixelColor(i, applyBrightness(color_rpm_mid));
     }
     else{
        strip2.setPixelColor(i, applyBrightness(color_rpm_low));
     }
  }
  strip2.show();
  
}

void displayDigit(int digit, int place, bool dp){

//  Serial.print("Displaying digit "); Serial.print(digit); Serial.print(" in place "); Serial.println(place);
//  resetDisplay();
  switch(digit){
    case 0:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, HIGH);
      digitalWrite(E_PIN, HIGH);
      digitalWrite(F_PIN, HIGH);
      digitalWrite(G_PIN, LOW);
      digitalWrite(DP_PIN, LOW);
      break;
    case 1:
      digitalWrite(A_PIN, LOW);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, LOW);
      digitalWrite(E_PIN, LOW);
      digitalWrite(F_PIN, LOW);
      digitalWrite(G_PIN, LOW);
      digitalWrite(DP_PIN, LOW);
      break;
    case 2:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, LOW);
      digitalWrite(D_PIN, HIGH);
      digitalWrite(E_PIN, HIGH);
      digitalWrite(F_PIN, LOW);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
    case 3:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, HIGH);
      digitalWrite(E_PIN, LOW);
      digitalWrite(F_PIN, LOW);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
    case 4:
      digitalWrite(A_PIN, LOW);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, LOW);
      digitalWrite(E_PIN, LOW);
      digitalWrite(F_PIN, HIGH);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
    case 5:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, LOW);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, HIGH);
      digitalWrite(E_PIN, LOW);
      digitalWrite(F_PIN, HIGH);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
    case 6:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, LOW);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, HIGH);
      digitalWrite(E_PIN, HIGH);
      digitalWrite(F_PIN, HIGH);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
    case 7:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, LOW);
      digitalWrite(E_PIN, LOW);
      digitalWrite(F_PIN, LOW);
      digitalWrite(G_PIN, LOW);
      digitalWrite(DP_PIN, LOW);
      break;
    case 8:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, HIGH);
      digitalWrite(E_PIN, HIGH);
      digitalWrite(F_PIN, HIGH);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
    case 9:
      digitalWrite(A_PIN, HIGH);
      digitalWrite(B_PIN, HIGH);
      digitalWrite(C_PIN, HIGH);
      digitalWrite(D_PIN, LOW);
      digitalWrite(E_PIN, LOW);
      digitalWrite(F_PIN, HIGH);
      digitalWrite(G_PIN, HIGH);
      digitalWrite(DP_PIN, LOW);
      break;
  }
  
  if(dp) digitalWrite(DP_PIN, HIGH);

  switch(place){
    case 0:
      digitalWrite(DIGIT_1_PIN, LOW);
      digitalWrite(DIGIT_2_PIN, HIGH);
      digitalWrite(DIGIT_3_PIN, HIGH);
      digitalWrite(DIGIT_4_PIN, HIGH);
      break;
    case 1:
      digitalWrite(DIGIT_1_PIN, HIGH);
      digitalWrite(DIGIT_2_PIN, LOW);
      digitalWrite(DIGIT_3_PIN, HIGH);
      digitalWrite(DIGIT_4_PIN, HIGH);
      break;
    case 2:
      digitalWrite(DIGIT_1_PIN, HIGH);
      digitalWrite(DIGIT_2_PIN, HIGH);
      digitalWrite(DIGIT_3_PIN, LOW);
      digitalWrite(DIGIT_4_PIN, HIGH);
      break;
    case 3:
      digitalWrite(DIGIT_1_PIN, HIGH);
      digitalWrite(DIGIT_2_PIN, HIGH);
      digitalWrite(DIGIT_3_PIN, HIGH);
      digitalWrite(DIGIT_4_PIN, LOW);
      break;
  }
}

void resetDisplay(){
    digitalWrite(DIGIT_1_PIN, HIGH);
    digitalWrite(DIGIT_2_PIN, HIGH);
    digitalWrite(DIGIT_3_PIN, HIGH);
    digitalWrite(DIGIT_4_PIN, HIGH);
    
    digitalWrite(A_PIN, LOW);
    digitalWrite(B_PIN, LOW);
    digitalWrite(C_PIN, LOW);
    digitalWrite(D_PIN, LOW);
    digitalWrite(E_PIN, LOW);
    digitalWrite(F_PIN, LOW);
    digitalWrite(G_PIN, LOW);
    digitalWrite(DP_PIN, LOW);
}

void updateDisplay(){

  if(currDigitIdx > 1){
    currDigitIdx = 0;
  }

  if(currDigitIdx == 0){
    getDigitsToDisplay(speedVar, digitsToDisplay);
    resetDisplay();

  }

  if(digitsToDisplay[0] < 0){
    resetDisplay();
  }
  else{
    int digit = digitsToDisplay[currDigitIdx];
    displayDigit(digit, currDigitIdx, false);
    currDigitIdx++;
  }
}


int getSpeedSerial(){
  String str = "";
  // int timeSinceInterrupt = Timer1.read();
  // Timer1.detachInterrupt();
  while(Serial.available()){
    char c = Serial.read();
    if(c == '\n') break;
    str += c;
    delay(1 );
  }
  
  // Timer1.attachInterrupt(updateDisplay, dispPeriod-timeSinceInterrupt);

  if(sizeof(str) > 0){
    Serial.print("Setting speed to "); Serial.println(str.toInt());
    return str.toInt();
  }
  else {
    return 0;
  }
}

void updateOBDValues(){

#ifdef TEST
  if(Serial.available()){
    int temp = getSpeedSerial();
//  if(temp > 0)
        speedVar = throttleVar = rpmVar = temp;
  }
#else
  int val = -10;
  byte pid;
  if(waitingForData){
    pid = PID_SPEED;
    switch(toReadNext){
      case TYPE_SPEED:
        if(obd.getResultNoBlock(pid, val)){\
          waitingForData = false;
          toReadNext = TYPE_THROTTLE;
         Serial.print("Recieved Data: "); Serial.println(val);
          speedVar = val / 1.60934;
        }
        else{
          failCount++;
          if(failCount > 100){
            waitingForData = false;
            toReadNext = TYPE_THROTTLE;
            failCount = 0;
          }
        }
        break;

      case TYPE_THROTTLE:
        pid = PID_THROTTLE;
        if(obd.getResultNoBlock(pid, val)){
          waitingForData = false;
          toReadNext = TYPE_RPM;
         Serial.print("Recieved Data: "); Serial.println(val);
          throttleVar = val;
        }
        else{
          failCount++;
          if(failCount > 100){
            waitingForData = false;
            toReadNext = TYPE_RPM;
            failCount = 0;
          }
        }
        break;

      
      case TYPE_RPM:
        pid = PID_RPM;
        if(obd.getResultNoBlock(pid, val)){
          waitingForData = false;
          toReadNext = TYPE_SPEED;
         Serial.print("Recieved Data: "); Serial.println(val);
          rpmVar = val;
        }
        else{
          failCount++;
          if(failCount > 100){
            waitingForData = false;
            toReadNext = TYPE_SPEED;
            failCount = 0;
          }
        }
        break;
    }

  }
  else{
    switch(toReadNext){
      case TYPE_SPEED:
        obd.readPIDAsync(PID_SPEED);
        waitingForData = true;
        break;
      case TYPE_THROTTLE:
        obd.readPIDAsync(PID_THROTTLE);
        waitingForData = true;
        break;
      case TYPE_RPM:
        obd.readPIDAsync(PID_RPM);
        waitingForData = true;
        break;
    }
      
  }
//  return -10;
#endif
  
}

uint32_t applyBrightness(color_t col){
  float k = col.alpha/255.0 * brightness;
  uint8_t r = col.red * k;
//  if(col.red > 0 && r == 0) r = 1;
  uint8_t g = col.green * k;
//  if(col.green > 0 && g == 0) g = 1;
  uint8_t b = col.blue * k;
//  if(col.blue > 0 && b == 0) b = 1;

//  Serial.print("r: "); Serial.print(r);
//  Serial.print(", g: "); Serial.print(g);
//  Serial.print(", b: "); Serial.print(b);
//  Serial.println();
  
  
  return strip1.Color(r, g, b);
}

void incBrightness(){
  brightness  = min(1, brightness*1.2);
  dispPeriod = max(max7SegmentPeriod, 250.0/brightness);
#ifdef TEST
  Serial.print("inc-ing brightness to "); Serial.println(brightness, 5);
#endif
}

void decBrightness(){
  brightness  = max(0, brightness/1.2);
  dispPeriod = max(max7SegmentPeriod, 250.0/brightness);
//  brightness -= 0.2;
#ifdef TEST
  Serial.print("dec-ing brightness to "); Serial.println(brightness, 5);
#endif
}

