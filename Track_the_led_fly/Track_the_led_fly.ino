#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#include "TimerOne.h"
//#include "TimerThree.h"
#include <stdio.h>
#include <stdlib.h>

#define primoLedVerde 7
#define secondoLedVerde 6
#define terzoLedVerde 5
#define quartoLedVerde 4

#define primoBottone 8
#define secondoBottone 9
#define terzoBottone 10
#define quartoBottone 11

#define potenziometro A0 
#define ledRosso 13

volatile uint8_t InterruptedPinShared;
volatile uint8_t PinStateShared;

unsigned char ledVerdi[4];
unsigned char bottoni[4];
unsigned int bottoniCliccati[4];
int currentLedOn, nextLedOn, temp, punteggio, frequenzaPotenziometro, level, levelGame;
boolean firstLedOn, checkCorrectClick, restartSystem;
long gameTime, initialGameTime, tempInitialGameTime;

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

  pinMode(potenziometro, INPUT);
  pinMode(ledRosso, OUTPUT);

  currentLedOn = 0;
  nextLedOn = 0;
  temp = 0;
  punteggio = 0;
  frequenzaPotenziometro = 0;
  level = 0;
  levelGame = 0;
  firstLedOn, checkCorrectClick = false;
  restartSystem = true;  
  initialGameTime, tempInitialGameTime, gameTime = 0;
  Serial.begin(9600);
}

void loop() {  
  if (!(restartSystem == true)){
  
    for(int i=0; i<4; i++){
      digitalWrite(ledVerdi[i], LOW);
    }
    
    static uint8_t InterruptedPin;
    static uint8_t PinState;
  
    //tempInitialGameTime = getLevel();
    tempInitialGameTime = 200;

    Serial.print("Initial game time:");
    Serial.println(initialGameTime);
    Serial.print("Temp game time:");
    Serial.println(tempInitialGameTime);
    Serial.print("Game time:");
    Serial.println(gameTime);
    
    if(tempInitialGameTime != initialGameTime){
      timesUp();
    }
    currentLedOn = flashLed();
    
    noInterrupts();      
     InterruptedPin = InterruptedPinShared;
     PinState = PinStateShared;
     //frequenzaPotenziometro = analogRead(potenziometro);
     //frequenzaPotenziometro = 200;
    interrupts();
  
    /*Serial.print("Livello: ");
    Serial.println(levelGame);
    
    Timer1.initialize(gameTime);
    Timer1.start();
    Timer1.attachInterrupt(timesUp, gameTime);

    Serial.print("Game time:");
    Serial.println(gameTime);*/
    
    for (int fadeValue = 0 ; fadeValue <= gameTime/2; fadeValue += (gameTime/2)/8) {
      analogWrite(ledVerdi[nextLedOn], fadeValue);
      delay(100);
    }
  
    for (int fadeValue = 255 ; fadeValue > gameTime/2; fadeValue -= (gameTime/2)/8) {
      analogWrite(ledVerdi[nextLedOn], fadeValue);
      delay(100);
    }  
  }else{

    for(int i=0; i<4; i++){
      digitalWrite(ledVerdi[i], LOW);
    }

    digitalWrite(ledRosso, HIGH);
    
    Serial.println("Hai 10 secondi per scegliere la difficoltà della partita!");
    delay(10000);
    digitalWrite(ledRosso, LOW);
    restartSystem = false;

    //initialGameTime = getLevel();
    initialGameTime = 200;
    gameTime = initialGameTime;
    tempInitialGameTime = initialGameTime;
    
  }
  
}

int flashLed() {

  if(firstLedOn == false){
    nextLedOn=0+rand()%4; //era 2
    firstLedOn = true;
  }else{
    
    if (rand () % 2 == 0){
      temp = currentLedOn++;
      
        while(currentLedOn != temp){
            temp++;
        }
        if(temp == 4){  //era 2
          nextLedOn = 0;
        }else{
          nextLedOn = temp;
        }
      
    }else{
      temp = currentLedOn--;
      
        while(currentLedOn != temp){
          if(temp == -1){
            temp = 3; //era 1
          }else{
            temp--;
          }
        }
        if(temp == -1){
          nextLedOn = 3;  //era 1
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

  /*Serial.print("Current Led On: ");
  Serial.println(currentLedOn);
  Serial.print("Clicked button: ");
  Serial.println(bottoni[InterruptedPinShared]);
  Serial.print("Interrupted Pin: ");
  Serial.println(InterruptedPinShared);

  for(int i=0; i<4; i++){
    if(bottoni[i] == InterruptedPinShared){
      Serial.println("taac");
      Serial.println(i);
    }
  }*/
  
  noInterrupts();
  checkCorrectClick = false;
  for(int i=0; i<4; i++){ 
    if(bottoni[i] == InterruptedPinShared && currentLedOn==i ){
          punteggio++;
          
          Serial.print("Punteggio: ");
          Serial.println(punteggio);
          checkCorrectClick = true;
          
          //finalGameTime=(gameTime/8)*7;
          gameTime = (gameTime/8)*7;

    }
  }
  if(checkCorrectClick == false){
    Serial.println("eeeeh volevi");
    timesUp();
  }
  interrupts();
}
int getLevel(){
  
  switch(frequenzaPotenziometro){
    case 0 ... 128:
      level = 1;
      initialGameTime = 8000000;
    break;
    
    case 129 ... 256:
      level = 2;
      initialGameTime = 7000000;
    break;
    
    case 257 ... 384:
      level = 3;
      initialGameTime = 6000000;
    break;
    
    case 385 ... 513:
      level = 4;
      initialGameTime = 5000000;
    break;

    case 514 ... 641:
      level = 5;
      initialGameTime = 4000000;
    break;

    case 642 ... 769:
      level = 6;
      initialGameTime = 3000000;
    break;

    case 770 ... 897:
      level = 7;
      initialGameTime = 2000000;
    break;

    case 898 ... 1023:
      level = 8;
      initialGameTime = 1000000;
    break;
    
  }

  return level;
}

void timesUp(){
  restartSystem = true;
  punteggio = 0;
}
