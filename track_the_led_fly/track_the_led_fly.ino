#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>

#include <stdio.h>
#include <stdlib.h>

#define primoLedVerde 7
//#define secondoLedVerde 6
#define terzoLedVerde 5
//#define quartoLedVerde 4

#define primoBottone 8
//#define secondoBottone 1
#define terzoBottone 2
//#define quartoBottone 3

#define potenziometro A0 
#define ledRosso 13

volatile uint8_t InterruptedPinShared;
volatile uint8_t PinStateShared;

unsigned char ledVerdi[4];
unsigned char bottoni[4];
unsigned int bottoniCliccati[4];
int currentLedOn, nextLedOn, temp, punteggio, frequenzaPotenziometro, level, levelGame;
boolean firstLedOn;

void setup() {
  ledVerdi[0] = primoLedVerde;
  ledVerdi[1] = 0;
  ledVerdi[2] = terzoLedVerde;
  ledVerdi[3] = 0;

  bottoni[0] = primoBottone;
  bottoni[1] = 0;
  bottoni[2] = terzoBottone;
  bottoni[3] = 0;

  for(int i=0; i<4; i++){
    pinMode(ledVerdi[i], OUTPUT);
    pinMode(bottoni[i], INPUT);
    enableInterrupt(bottoni[i], incPunteggio, RISING);
  }

  pinMode(potenziometro, INPUT);
  pinMode(ledRosso, OUTPUT);

  currentLedOn = 0;
  nextLedOn = 0;
  temp = 0;
  punteggio = 0;
  frequenzaPotenziometro = 0;
  level = 0;
  levelGame = 0;
  firstLedOn = false;  
  digitalWrite(ledRosso, HIGH);
  Serial.begin(9600);

}

void loop() {

  if !(digitalRead(ledRosso) == HIGH){
  
    for(int i=0; i<4; i++){
      digitalWrite(ledVerdi[i], LOW);
    }
  
    static uint8_t InterruptedPin;
    static uint8_t PinState;
  
    levelGame = getLevel();
    currentLedOn = flashLed();
    
    noInterrupts();      
     InterruptedPin = InterruptedPinShared;
     PinState = PinStateShared;
     frequenzaPotenziometro = analogRead(potenziometro);
    interrupts();
  
    Serial.print("Livello: ");
    Serial.println(levelGame);
  
    for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 15) {
      analogWrite(ledVerdi[nextLedOn], fadeValue);
      delay(50);
    }
  
    for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 15) {
      analogWrite(ledVerdi[nextLedOn], fadeValue);
      delay(50);
    }  
  }else{
    
    //10 secondi per decidere il livello
    delay(10000);
    digitalWrite(ledRosso, LOW);
    
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
        if(temp == 2){  //era 4
          nextLedOn = 0;
        }else{
          nextLedOn = temp;
        }
      
    }else{
      temp = currentLedOn--;
      
        while(currentLedOn != temp){
          if(temp == -1){
            temp = 1; //era 3
          }else{
            temp--;
          }
        }
        if(temp == -1){
          nextLedOn = 1;  //era 3
        }else{
          nextLedOn = temp;
        }
      
    }
  }
  return nextLedOn;
}

void incPunteggio(){
  
  InterruptedPinShared=arduinoInterruptedPin;
  PinStateShared=arduinoPinState;
  
  Serial.println(currentLedOn);
  Serial.println(bottoni[InterruptedPinShared]);
  
  noInterrupts();
  for(int i=0; i<4; i++){
    if(bottoni[i]==bottoni[InterruptedPinShared] && currentLedOn==i ){
          punteggio++;
          Serial.print("pin: ");
          Serial.println(InterruptedPinShared);
          Serial.print("Punteggio: ");
          Serial.println(punteggio);
    }
  }
  interrupts();
}

void redLedGone(){
  Serial.println("Ci sono");
}

int getLevel(){

  Serial.println(frequenzaPotenziometro);
  
  switch(frequenzaPotenziometro){
    case 0 ... 128:
      level = 1;
    break;
    
    case 129 ... 256:
      level = 2;
    break;
    
    case 257 ... 384:
      level = 3;
    break;
    
    case 385 ... 513:
      level = 4;
    break;

    case 514 ... 641:
      level = 5;
    break;

    case 642 ... 769:
      level = 6;
    break;

    case 770 ... 897:
      level = 7;
    break;

    case 898 ... 1023:
      level = 8;
    break;
    
  }

  return level;
}
