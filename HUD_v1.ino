#include <Adafruit_NeoPixel.h>
  
#define LED_PIN 9
#define BTN_BRIGHTER 2
#define BTN_DIMMER 3

const int mphPerLED = 2;
const int tickCount = 6;
uint8_t tickLEDs[tickCount] = {4,9,14,19,24,29};
//uint8_t tickLEDs[tickCount] = {2,5,8,11,14,17,20,23,26,29};

int speedVar = 0;

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

////color_t colorTick1(255,0,128,128);
//color_t colorTick1(255,64,32,128);
//color_t colorTick2(255,64,32,128);
////color_t colorTick3(0,255,128,128);
//color_t colorTick3(255,64,32,128);
//color_t colorTickSet1(255,255,128,255);
//color_t colorTickSet2(255,255,128,255);
//color_t colorTickSet3(255,255,128,255);
//color_t colorSet1(255,0,0,255);
//color_t colorSet2(255,255,0,255);
//color_t colorSet3(0,255,0,255);
//color_t colorOff(0,0,0,0);

color_t colorTick1(255,0,0,128);
color_t colorTick2(255,128,0,128);
color_t colorTick3(64,255,32,128);
//color_t colorTickSet1(255,255,128,64);
//color_t colorTickSet2(255,255,128,64);
//color_t colorTickSet3(255,255,128,64);
color_t colorTickSet1(255,16,8,16);
color_t colorTickSet2(255,20,8,16);
color_t colorTickSet3(255,20,8,16);
color_t colorSet1(255,12,0,255);
color_t colorSet2(255,12,0,255);
color_t colorSet3(255,12,0,255);
color_t colorSpecial(255,255,255,128);
color_t colorSpecialSet(255,64,64,128);
color_t colorOff(0,0,0,0);


float brightness = 0.02;
int incDebounce = 0;
int decDebounce = 0;


void setup(){
    Serial.begin(38400);
    ledStrip.begin();
    ledStrip.show();

  for(int i=0; i<ledStrip.numPixels(); i++){
      if(i+1 == 20)
        ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
      else if (isTick(i))
        ledStrip.setPixelColor(i, applyBrightness(colorTick1));
    }
    ledStrip.show();


  pinMode(BTN_BRIGHTER, INPUT_PULLUP);
  pinMode(BTN_DIMMER, INPUT_PULLUP);
  delay(500);
  attachInterrupt(digitalPinToInterrupt(BTN_BRIGHTER), incBrightness, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DIMMER), decBrightness, FALLING);
}

void loop() {
//    setSpeed(speedVar);
    int temp = getSpeed();
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
          if(i+1 == 20)
            ledStrip.setPixelColor(i, applyBrightness(colorSpecialSet));
          else if(isTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorTickSet1));
          else
            ledStrip.setPixelColor(i, applyBrightness(colorSet1));
          break;
        case 1:
          if(i+31 == 55)
            ledStrip.setPixelColor(i, applyBrightness(colorSpecialSet));
          else if(isTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorTickSet2));
          else
            ledStrip.setPixelColor(i, applyBrightness(colorSet2));
          break;
        case 2:
          if(i+61 == 70)
            ledStrip.setPixelColor(i, applyBrightness(colorSpecialSet));
          else if(isTick(i))
            ledStrip.setPixelColor(i, applyBrightness(colorTickSet3));
          else
            ledStrip.setPixelColor(i, applyBrightness(colorSet3));
          break;
      }
        
    }
    else{
      if(isTick(i)){
        switch(level){
          case 0:
            if(i+1 == 20)
              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
            else 
              ledStrip.setPixelColor(i, applyBrightness(colorTick1));
            break;
          case 1:
            if(i+31 == 55)
              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
            else 
              ledStrip.setPixelColor(i, applyBrightness(colorTick2));
            break;
          case 2:
            if(i+61 == 70)
              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
            else 
              ledStrip.setPixelColor(i, applyBrightness(colorTick3));
            break;
        }
      }
      else if(level > 2){
        ledStrip.setPixelColor(i, applyBrightness(colorSet3));
      }
      else{
        switch(level){
          case 0:
            if(i+1 == 20)
              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
            else 
              ledStrip.setPixelColor(i, applyBrightness(colorOff));
            break;
          case 1:
            if(i+31 == 55)
              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
            else 
              ledStrip.setPixelColor(i, applyBrightness(colorOff));
            break;
          case 2:
            if(i+61 == 70)
              ledStrip.setPixelColor(i, applyBrightness(colorSpecial));
            else 
              ledStrip.setPixelColor(i, applyBrightness(colorOff));
            break;
        }
      }
    }
  }
  ledStrip.show();
}

uint32_t applyBrightness(color_t col){
  float a = col.alpha/255.0 * brightness;
  uint8_t r = col.red * a;
  if(col.red > 0 && r ==0) r == 1;
  uint8_t g = col.green * a;
  if(col.green > 0 && g ==0) g == 1;
  uint8_t b = col.blue * a;
  if(col.blue > 0 && b ==0) b == 1;
  
  return ledStrip.Color(r, g, b);
}

int getSpeed(){
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

bool isTick(int led){
  for(int i=0; i<tickCount; i++){
    if(led == tickLEDs[i]) return true;
  }
  return false;
}

void incBrightness(){
  brightness  = min(1, brightness+.02);
  Serial.print("inc-ing brightness to "); Serial.println(brightness);
}

void decBrightness(){
  brightness  = max(0, brightness-.02);
  Serial.print("dec-ing brightness to "); Serial.println(brightness);
}

