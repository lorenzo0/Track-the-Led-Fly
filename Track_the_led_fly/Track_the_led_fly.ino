#define EI_ARDUINO_INTERRUPTED_PIN
#include "EnableInterrupt.h"
#include "TimerOne.h"
#include "TimerThree.h"

#include <stdio.h>
#include <stdlib.h>

#define primoLedVerde 6
#define secondoLedVerde 5
#define terzoLedVerde 4
#define quartoLedVerde 3

#define primoBottone 12
#define secondoBottone 11
#define terzoBottone 10
#define quartoBottone 9

#define ledRosso 7

unsigned char ledVerdi[4];
unsigned char bottoni[4];
unsigned int bottoniCliccati[4];
int currentLedOn, nextLedOn, temp, punteggio, catchButton;
boolean firstLedOn;

volatile uint8_t InterruptedPinShared;
volatile uint8_t PinStateShared;
volatile unsigned long timerToCatch = 0;

void setup() {
  
  ledVerdi[0] = primoLedVerde;
  ledVerdi[1] = secondoLedVerde;
  ledVerdi[2] = terzoLedVerde;
  ledVerdi[3] = quartoLedVerde;

  bottoni[0] = primoBottone;
  bottoni[1] = secondoBottone;
  bottoni[2] = terzoBottone;
  bottoni[3] = quartoBottone;

  for(int i=0; i<4; i++){
    pinMode(ledVerdi[i], OUTPUT);
    pinMode(bottoni[i], INPUT);
    enableInterrupt(bottoni[i], incPunteggio, RISING);
  }

  pinMode(ledRosso, OUTPUT);

  /*
    Timer1 --> Tempo per catturare la mosca (8 bit)
    Timer3 --> Tempo totale di gioco (16bit)  
  */
  Timer1.initialize(1000000);
  Timer3.initialize(150000);
  Timer3.attachInterrupt(incPunteggio);

  currentLedOn = 0;
  nextLedOn = 0;
  temp = 0;
  punteggio = 0;
  catchButton = 0;
  firstLedOn = false;  
  Serial.begin(9600);

  Timer1.start();
}

void loop() {
  
  for(int i=0; i<4; i++){
    digitalWrite(ledVerdi[i], LOW);
  }

  currentLedOn = flashLed();
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 15) {
    analogWrite(ledVerdi[nextLedOn], fadeValue);
    delay(50);
  }

  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 15) {
    analogWrite(ledVerdi[nextLedOn], fadeValue);
    delay(50);
  }  
}

int flashLed() {

  if(firstLedOn == false){
    nextLedOn=0+rand()%4;
    firstLedOn = true;
  }else{
    
    if (rand () % 2 == 0){
      temp = currentLedOn++;
      
        while(currentLedOn != temp){
            temp++;
        }
        if(temp == 4){
          nextLedOn = 0;
        }else{
          nextLedOn = temp;
        }
      
    }else{
      temp = currentLedOn--;
      
        while(currentLedOn != temp){
          if(temp == -1){
            temp = 3;
          }else{
            temp--;
          }
        }
        if(temp == -1){
          nextLedOn = 3;
        }else{
          nextLedOn = temp;
        }
      
    }
  }
  return nextLedOn;
}

void incPunteggio(){

  static uint8_t InterruptedPin;
  static uint8_t PinState;
  static uint8_t TimerThreeStopped;
  
  noInterrupts();      // turn interrupts off quickly while we take local copies of the shared variables
   InterruptedPin = InterruptedPinShared;
   PinState = PinStateShared;
   TimerThreeStopped = timerToCatch;
  interrupts();

  Serial.println(PinState);
  Serial.println(InterruptedPin);
  Serial.println(TimerThreeStopped);
  
  if(currentLedOn == catchButton){
        punteggio++;
  }

  Timer1.restart();
  catchButton = 0;
}
