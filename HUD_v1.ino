#include <OBD2UART.h>

#define TEST

#define DIGIT_1_PIN 6
//#define DIGIT_2_PIN 8
//#define DIGIT_3_PIN 9
//#define DIGIT_4_PIN 12
//
//#define A_PIN 11 A2
//#define B_PIN 7
//#define C_PIN 4
//#define D_PIN A2
//#define E_PIN A1
//#define F_PIN 10
//#define G_PIN 5
//#define DP_PIN A3

#define DIGIT_1_PIN 6
#define DIGIT_2_PIN 8
#define DIGIT_3_PIN 9
#define DIGIT_4_PIN 12

#define A_PIN A2
#define B_PIN 4
#define C_PIN 7
#define D_PIN 11
#define E_PIN 10
#define F_PIN A1
#define G_PIN 5
#define DP_PIN A3

  
//#define LED_PIN 9
#define BTN_BRIGHTER 2
#define BTN_DIMMER 3

int speedVar = 8888;
float brightness = 0.1;
bool waitingForData = false;
int failCount = 0;

COBD obd;



void setup(){
  
#ifdef TEST
  Serial.begin(38400);
#endif

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
  
  for(int i=4000; i>0; i--){
    displayDigit(0,0,false);
    displayDigit(0,1,false);
    displayDigit(0,2,false);
    displayDigit(0,3,false);
  }
  resetDisplay();

//  Serial.println("beginning...");

#ifndef TEST
  obd.begin();
#endif

//  Serial.println("initializing...");
 
#ifndef TEST
  while (!obd.init());
#endif

//  Serial.println("Done");
  pinMode(BTN_BRIGHTER, INPUT_PULLUP);
  pinMode(BTN_DIMMER, INPUT_PULLUP);
  delay(500);
  attachInterrupt(digitalPinToInterrupt(BTN_BRIGHTER), incBrightness, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DIMMER), decBrightness, FALLING);

 
}

void loop() {
  
#ifdef TEST
    int temp = getSpeedSerial();
#else
    int temp = getSpeedOBD() / 1.60934;
#endif

    if(temp >= 0){
      speedVar = temp;
    }
    setSpeed(speedVar);
    delayMicroseconds(150);
    resetDisplay();

    delayMicroseconds(100/brightness);
}

void setSpeed(int speed){

//  Serial.print(digits); Serial.print(" digits: ");

  if(speed == 0){
    displayDigit(0,0,false);
  }
  else{
    int digits = log10(speed) + 1;
    for(int i=0; i<digits; i++){
      int digit = (int)(speed/(pow(10,i)) ) % 10;
  //    Serial.print(digit); Serial.print(" - ");
      displayDigit(digit, i, false);
    }
  }


//  Serial.println();
 
}


void displayDigit(int digit, int place, bool dp){

//  Serial.print("Displaying digit "); Serial.print(digit); Serial.print(" in place "); Serial.println(place);
  resetDisplay();
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
  int val = -20;

  if(waitingForData){
      byte pid = PID_SPEED;
//      if(obd.getResult(pid, val)){
      if(obd.getResultNoBlock(pid, val)){
        waitingForData = false;
//        Serial.print("Recieved Data: "); Serial.println(val);
        return val;
//           return 55;
      }
      else{
        failCount++;
        if(failCount > 100){
          waitingForData = false;
          failCount = 0;
        }
      }
  }
  else{
    obd.readPIDAsync(PID_SPEED);
    waitingForData = true;

      

//      if(obd.readPID(PID_SPEED, val)){
//
//        Serial.print("read from obd port: "); Serial.println(val);
//        return val;
//      }

//      Serial.println("Read failed");
      
  }
  return -10;
}

void incBrightness(){
//  brightness  = min(1, brightness*1.2);
  brightness += 0.2;
#ifdef TEST
  Serial.print("inc-ing brightness to "); Serial.println(brightness, 5);
#endif
}

void decBrightness(){
//  brightness  = max(0, brightness/1.2);
  brightness -= 0.2;
#ifdef TEST
  Serial.print("dec-ing brightness to "); Serial.println(brightness, 5);
#endif
}

