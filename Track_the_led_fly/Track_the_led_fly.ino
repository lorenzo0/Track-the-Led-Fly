/*
 * Authors: Castelli Giorgia 873787
 *          Pisanò Lorenzo  900590
 *          
   * Subject: Track the led fly
*/

#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#include "TimerOne.h"
#include <stdio.h>
#include <stdlib.h>

#define greenLED1 10
#define greenLED2 9
#define greenLED3 6
#define greenLED4 5

#define button1 4
#define button2 7
#define button3 8
#define button4 12

#define potentiometer A0 
#define redLED 11

/* Costante da definire per la definizione del tempo */
const long k=0.8;

volatile uint8_t InterruptedPinShared;
volatile uint8_t PinStateShared;

unsigned char greenLEDs[4];
unsigned char buttons[4];

int currentLedOn, nextLedOn, temp, score, frequencyPot, level, levelGame;
boolean firstLedOn, checkCorrectClick, restartSystem, firstStart;
long gameTime, initialGameTime, randomGameTime, tempInitialGameTime, microGameTime;

/* 
 *  La procedura setup serve ad inizializzare tutti i componenti e variabili
 *  necessarie per il corretto funzionamento del gioco. Vengono creati degli 
 *  array per memorizzare i quattro bottoni ed i quattro led verdi. Aumentiamo così
 *  la compattezza del programma.
*/
void setup() {
  greenLEDs[0] = greenLED1;
  greenLEDs[1] = greenLED2;
  greenLEDs[2] = greenLED3;
  greenLEDs[3] = greenLED4;

  buttons[0] = button1;
  buttons[1] = button2;
  buttons[2] = button3;
  buttons[3] = button4;

  for(int i=0; i<4; i++){
    pinMode(greenLEDs[i], OUTPUT);
    pinMode(buttons[i], INPUT);
    enableInterrupt(buttons[i], incPunteggio, RISING);
  }

  pinMode(potentiometer, INPUT);
  pinMode(redLED, OUTPUT);

  currentLedOn, nextLedOn = -1;
  temp, score = 0;
  frequencyPot = 0;
  level, levelGame = 0;
  initialGameTime, tempInitialGameTime, gameTime, microGameTime = 0;
  
  firstLedOn, checkCorrectClick = false;
  restartSystem, firstStart = true;  
  
  Serial.begin(9600);
  Serial.println("Welcome to the Track to Led Fly Game. Press Key T1 to Start");

  Timer1.initialize();
}

/*
 * 
 *  Nel superloop di arduino andiamo a sviluppare i comportamenti principali che
 *  quest'ultimo deve avere durante la sua esecuzione.
 *  
 *  FirstStart è una variabile boolena che controlla se è la prima volta che il sistema
 *  è avviato oppure è già stato in esecuzione durante il passato.
 *  Se il risultato è positivo, quindi è la prima volta che avviamo il programma, allora vengono
 *  rispettate le richieste della consegna, altrimenti procediamo con la normale esecuzione del gioco.
 *  
 *  RestartBoolean anche è una variabile booleana che ha l'onere di controllare se il bottone
 *  non è stato cliccato entro il range [di tempo tMin - (tMin*k)] oppure se è stato premuto il pulsante
 *  tattile errato. In questi due casi, il gioco va in stato -Game over- e ricomincia.
 *  
 *  La normale esecuzione del programma coinvolge la procedura flashLed che va a determinare
 *  quale dei prossimi led sarà prossimo acceso. Viene utilizzato il costrutto interrupts per permettere 
 *  al programma di memorizzare nelle variabili volatile i dati necessarie per gestire le
 *  possibili interruzioni che sono avvenute nei momenti precedenti.
 *  
 *  La consegna chiede inoltre che i led debbano - pulsare -. Viene dunque interpretata la
 *  richiesta di fading dei led, gestita tramite un passaggio di dati analogici (0-255) che vanno
 *  a definire la lucentezza del diodo.
 *  
 *  -------
 *  
 *  Nel momento in cui il sistema viene riavviato, il led rosso viene illuminato per 2 secondi
 *  - come richiesto - ed il gioco riparte, analizzando il valore corrente del potenziometro.
 *  
*/

void loop() {
  
  if (!(firstStart == false))
    initialGameState();
  else{
    if (!(restartSystem == true)){
    
      for(int i=0; i<4; i++){
        digitalWrite(greenLEDs[i], LOW);
      }
      
      static uint8_t InterruptedPin;
      static uint8_t PinState;
  
      currentLedOn = flashLed();
      
      noInterrupts();      
       InterruptedPin = InterruptedPinShared;
       PinState = PinStateShared;
      interrupts();

      microGameTime = gameTime*10000;
      Timer1.setPeriod(microGameTime);
        
      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 15) {
        analogWrite(greenLEDs[nextLedOn], fadeValue);
        delay(gameTime/2);
      }
      
      for (int fadeValue = 255 ; fadeValue > 0; fadeValue -= 15) {
        analogWrite(greenLEDs[nextLedOn], fadeValue);
        delay((gameTime/2));
      }  

      Timer1.attachInterrupt(timesUp);
      
    }else{
  
      for(int i=0; i<4; i++){
        digitalWrite(greenLEDs[i], LOW);
      }
  
      digitalWrite(redLED, HIGH);
          
      delay(2000);
      
      digitalWrite(redLED, LOW);
      restartSystem = false;
  
      frequencyPot = analogRead(potentiometer);
      getLevel();
      gameTime = initialGameTime;
      tempInitialGameTime = initialGameTime;
      
    }
  }
  
}

/*
 * 
 * Procedura che gestisce lo stato iniziale - quindi - quando arduino è stato
 * appena alimentato ed il programma è stato mandato in esecuzione (tramite cache
 * o compilatore)
 * Il led rosso pulsa ad intervalli regolari finchè il pulsante tattile T1 non 
 * viene premuto.
 * 
*/

void initialGameState(){

  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 15) {
      analogWrite(redLED, fadeValue);
      delay(60);
  }

  for (int fadeValue = 255 ; fadeValue > 0; fadeValue -= 15) {
      analogWrite(redLED, fadeValue);
      delay(60);
  }  
}

/*
 * 
 * Procedura che va a gestire quale led (corrente e prossimo)
 * deve essere acceso. Questo viene trovato a seguito di alcuni vincoli
 * posti dalla consegna (Es. Adiacenza tra corrente - prossimo)
 * 
*/
int flashLed() {

  checkCorrectClick = false;

  if(firstLedOn == false){
    nextLedOn=0+rand()%4;
    firstLedOn = true;
  }else{
    
    if (rand () % 2 == 0){
      temp = currentLedOn++;
      
        while(currentLedOn != temp){
            temp++;
        }
        if(temp == 4)
          nextLedOn = 0;
        else
          nextLedOn = temp;
        
      
    }else{
      temp = currentLedOn--;
      
        while(currentLedOn != temp){
          if(temp == -1)
            temp = 3; 
          else
            temp--;
        }
        if(temp == -1)
          nextLedOn = 3; 
        else
          nextLedOn = temp;      
    }
  }
  return nextLedOn;
}

/*
 * 
 * Procedura che va a gestire il random time che va a costituire
 * il gameTime per il prossimo periodo di tempo di accensione del led.
 * 
 * Formula --> min + rand() % (max+1 - min);
 * 
*/
void randomTime(){
  gameTime = (gameTime)+rand()%((gameTime*k)- gameTime);
}

/*
 * 
 * incPunteggio nasce come procedura che viene richiamata nel momento in cui
 * si verifica un'interruzione da parte di uno dei pulsanti tattili.
 * 
 * enableInterrupt(buttons[i], incPunteggio, RISING);
 * 
 * Questo, prendendo il valore assegnato nel loop alle variabili volatile,
 * analizza la posizione del pulsante premuto e del led correntemente attivo.
 * Se questi corrispondono, va ad attribuire un punto aggiuntivo alla partita.
 * 
 * Viene scelto di utilizzare anche in questo caso il costrutto noInterrupts()
 * per evitare di avere problemi di concorrenza durante la gestione del punteggio
 * e l'assegnazione del nuovo tempo di gioco.
 * 
 * Inoltre viene fermato Timer1 nel caso in cui il bottone cliccato sia quello
 * giusto, riparte poi nel loop con Time1.setPeriod().
 * 
 * 
*/
void incPunteggio(){
  
  InterruptedPinShared=arduinoInterruptedPin;
  PinStateShared=arduinoPinState;

  if(firstStart == true & buttons[0] == InterruptedPinShared){
    firstStart = false;
    Serial.println("GO!");
  }else{
    noInterrupts();
    for(int i=0; i<4; i++){ 
      if(buttons[i] == InterruptedPinShared && currentLedOn==i && restartSystem == false ){
            score++;

            Timer1.stop();
  
            Serial.print("Tracking the fly: pos ");
            Serial.println(currentLedOn);
            checkCorrectClick = true;
            
            gameTime = (gameTime/8)*7;            
            randomTime();
      }
    }
    interrupts();
  }
}

/*
 * 
 * Procedura che, preso in input il valore analogico del
 * potenziometro 10K, attraverso un costrutto switch
 * va a definire la difficoltà della
 * partita ed il tempo iniziale di gioco.
 * 
*/
int getLevel(){
  
  switch(frequencyPot){
    case 0 ... 128:
      level = 1;
      initialGameTime = 800;
    break;
    
    case 129 ... 256:
      level = 2;
      initialGameTime = 700;
    break;
    
    case 257 ... 384:
      level = 3;
      initialGameTime = 600;
    break;
    
    case 385 ... 513:
      level = 4;
      initialGameTime = 500;
    break;

    case 514 ... 641:
      level = 5;
      initialGameTime = 400;
    break;

    case 642 ... 769:
      level = 6;
      initialGameTime = 300;
    break;

    case 770 ... 897:
      level = 7;
      initialGameTime = 200;
    break;

    case 898 ... 1023:
      level = 8;
      initialGameTime = 100;
    break;
    
  }

  return level;
}

/*
 * 
 * Procedura che, quando viene richiamata, segnala al superloop
 * che è necessaria un'interruzione di partita a seguito di un 
 * evento (il bottone non è stato cliccato entro il range [di tempo tMin - (tMin*k)] 
 * oppure se è stato premuto il pulsante tattile errato).
 * 
 * Ad ogni partita, viene azzerato il punteggio.
 * 
*/
void timesUp(){
  restartSystem = true;
  Serial.print("Game Over - Score: ");
  Serial.println(score);
  score = 0;
}
