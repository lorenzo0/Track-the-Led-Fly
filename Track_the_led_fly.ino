#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#define ENABLE_INT_IN_PIN 9

#include <stdio.h>
#include <stdlib.h>

#define primoLedVerde 6
#define secondoLedVerde 5
#define terzoLedVerde 4
#define quartoLedVerde 3

#define primoBottone 12
#define secondoBottone 13
#define terzoBottone 10
#define quartoBottone 8

unsigned char ledVerdi[4];
unsigned char bottoni[4];
unsigned int bottoniCliccati[4];
int currentLedOn, nextLedOn, temp, punteggio, catchButton;
boolean firstLedOn;

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

  currentLedOn = 0;
  nextLedOn = 0;
  temp = 0;
  punteggio = 0;
  catchButton = 0;
  firstLedOn = false;  
  Serial.begin(9600);

}

void loop() {

  /*for(int i=0; i<4; i++){
    bottoniCliccati[i] = digitalRead(bottoni[i]);
  }

  /*bottoniCliccati[3] = digitalRead(bottoni[3]);
  Serial.println(bottoniCliccati[3]);

  if(bottoniCliccati[3] == 1){
    //count++;
    digitalWrite(ledVerdi[3], HIGH);
  }

  digitalWrite(ledVerdi[3],LOW);
  bottoniCliccati[1] = digitalRead(bottoni[1]);
  bottoniCliccati[2] = digitalRead(bottoni[2]);
  bottoniCliccati[3] = digitalRead(bottoni[3]);
  */
  
  for(int i=0; i<4; i++){
    digitalWrite(ledVerdi[i], LOW);
  }

  currentLedOn = flashLed();
  digitalWrite(ledVerdi[nextLedOn], HIGH);
  //Serial.println(punteggio);
  delay(1000);
  
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
  

  for(int i=0; i<4; i++){
    bottoniCliccati[i] = digitalRead(bottoni[i]);
    if(bottoniCliccati[i] == 1){
       catchButton = i;
    }
  }
  
  if(currentLedOn == catchButton){
        punteggio++;
  }
  Serial.println(catchButton);
  Serial.println(punteggio);
}
