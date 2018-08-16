#include <Adafruit_NeoPixel.h>
#include <OBD2UART.h>

//#define TEST
  
#define LED_PIN 9
#define BTN_BRIGHTER 2
#define BTN_DIMMER 3

const int mphPerLED = 2;
const int tickCount = 4;
uint8_t majorTicks[tickCount] = {-1,9,19,29};
uint8_t minorTicks[tickCount] = {4,13,14,24};

int speedVar = 0;

COBD obd;
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(30, LED_PIN, NEO_GRB + NEO_KHZ800);

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

color_t colorMajorTick1(255,0,255,128);
color_t colorMajorTick2(64,255,32,128);
color_t colorMajorTick3(255,0,0,128);
color_t colorMinorTick3(255,255,255,128);
color_t colorMinorTick1(255,255,255,128);
color_t colorMinorTick2(255,255,255,128);
color_t colorMajorTickSet1(255,20,8,64);
color_t colorMajorTickSet2(255,20,8,64);
color_t colorMajorTickSet3(255,20,8,64);
color_t colorMinorTickSet1(255,20,8,64);
color_t colorMinorTickSet2(255,20,8,64);
color_t colorMinorTickSet3(255,20,8,64);
color_t colorSet1(255,12,0,255);
color_t colorSet2(255,12,0,255);
color_t colorSet3(255,12,0,255);
//color_t colorSpecial(255,128,0,128);
//color_t colorSpecialSet(255,64,64,128);
color_t colorOff(0,0,0,0);


float brightness = 0.02;
int incDebounce = 0;
int decDebounce = 0;


void setup(){
#ifdef TEST
  Serial.begin(38400);
#endif
  ledStrip.begin();
  ledStrip.show();
  
  color_t colorSpecial(255,128,0,128);
  for(int i=0; i<10; i++){
      ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
  }
  ledStrip.show();

#ifndef TEST
  obd.begin();
#endif
  
  for(int i=10; i<20; i++){
      ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
  }
  ledStrip.show();

#ifndef TEST
  while (!obd.init());
#endif
  
  for(int i=20; i<30; i++){
      ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
  }
  ledStrip.show();


  pinMode(BTN_BRIGHTER, INPUT_PULLUP);
  pinMode(BTN_DIMMER, INPUT_PULLUP);
  delay(500);
  attachInterrupt(digitalPinToInterrupt(BTN_BRIGHTER), incBrightness, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DIMMER), decBrightness, FALLING);
  
  for(int i=0; i<ledStrip.numPixels(); i++){
    /*if(i+1 == 15)
      ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
    else*/ if (isMajorTick(i))
      ledStrip.setPixelColor(i, applyBrightness(colorMajorTick1));
    else if (isMinorTick(i))
      ledStrip.setPixelColor(i, applyBrightness(colorMinorTick1));
    else 
      ledStrip.setPixelColor(i, applyBrightness(colorOff));
  }
  ledStrip.show();
}

void loop() {
  
#ifdef TEST
    int temp = getSpeedSerial();
#else
    int temp = getSpeedOBD() / 1.60934;
#endif

    if(temp != 0){
      speedVar = temp;
    }
    setSpeed(speedVar);

    delay(200);
}

void setSpeed(int speed){
//  Serial.print("Speed = "); Serial.print(speed);
  int level = speed/30;
//  Serial.print(", level = "); Serial.print(level);
  speed %= 30;
//  Serial.print(", ledCount = "); Serial.println(speed);

 
  for(int i=0; i<ledStrip.numPixels(); i++){
    if (i < speed){
      switch(level){
        case 0:
          /*if(i+1 == 15)
            ledStrip.setPixelColor(i, applyBrightness(colorSpecialSet));
          else */if(isMajorTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorMajorTickSet1));
          else if(isMinorTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorMinorTickSet1));
          else
            ledStrip.setPixelColor(i, applyBrightness(colorSet1));
          break;
        case 1:
          /*if(i+31 == 55)
            ledStrip.setPixelColor(i, applyBrightness(colorSpecialSet));
          else*/ if(isMajorTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorMajorTickSet2));
          else if(isMinorTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorMinorTickSet2));
          else
            ledStrip.setPixelColor(i, applyBrightness(colorSet2));
          break;
        case 2:
          /*if(i+61 == 70)
            ledStrip.setPixelColor(i, applyBrightness(colorSpecialSet));
          else*/ if(isMajorTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorMajorTickSet3));
          else if(isMinorTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorMinorTickSet3));
          else
            ledStrip.setPixelColor(i, applyBrightness(colorSet3));
          break;
      }
        
    }
    else{
      if(isMajorTick(i)){
        switch(level){
          case 0:
//            if(i+1 == 20)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorMajorTick1));
            break;
          case 1:
//            if(i+31 == 55)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorMajorTick2));
            break;
          case 2:
//            if(i+61 == 70)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorMajorTick3));
            break;
        }
      }
      else if(isMinorTick(i)){
        switch(level){
          case 0:
//            if(i+1 == 20)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorMinorTick1));
            break;
          case 1:
//            if(i+31 == 55)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorMinorTick2));
            break;
          case 2:
//            if(i+61 == 70)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorMinorTick3));
            break;
        }
      }
      else if(level > 2){
        ledStrip.setPixelColor(i, applyBrightness(colorSet3));
      }
      else{
        switch(level){
          case 0:
//            if(i+1 == 20)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorOff));
            break;
          case 1:
//            if(i+31 == 55)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorOff));
            break;
          case 2:
//            if(i+61 == 70)
//              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
//            else 
              ledStrip.setPixelColor(i, applyBrightness(colorOff));
            break;
        }
      }
    }
  }
  ledStrip.show();
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
  
  
  return ledStrip.Color(r, g, b);
}

int getSpeedSerial(){
  String str = "";
  while(Serial.available()){
    char c = Serial.read();
    if(c == '\n') break;
    str += c;
  }

  if(sizeof(str) > 0){
    return str.toInt();
  }
  else {
    return 0;
  }
}

int getSpeedOBD(){
  int val = 0;
  if (obd.readPID(PID_SPEED, val)) {
    // light on LED on Arduino board when the RPM exceeds 3000
    return val;
  }
  return 0;
}

bool isMajorTick(int led){
  for(int i=0; i<tickCount; i++){
    if(led == majorTicks[i]) return true;
  }
  return false;
}

bool isMinorTick(int led){
  for(int i=0; i<tickCount; i++){
    if(led == minorTicks[i]) return true;
  }
  return false;
}

void incBrightness(){
  brightness  = min(1, brightness*1.2);
#ifdef TEST
  Serial.print("inc-ing brightness to "); Serial.println(brightness, 5);
#endif
}

void decBrightness(){
  brightness  = max(0, brightness/1.2);
#ifdef TEST
  Serial.print("dec-ing brightness to "); Serial.println(brightness, 5);
#endif
}

