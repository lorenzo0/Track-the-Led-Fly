/*
 * 
 * Authors: Castelli Giorgia  873787
 *          Pisanò Lorenzo    900590
 *          
 * Subject: Track the led fly
 * 
 * Link al video - presente anche nel file txt visibile all'interno dello zip:
 * https://liveunibo-my.sharepoint.com/:v:/g/personal/lorenzo_pisano_studio_unibo_it/EbdGGxWgiJBNltQspmakqUUB-7LQsHrXbJ_5I_59VYZ2cQ?e=qASkHn
 * - visibile solo da account accademici con dominio @unibo -
 * 
*/

#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#include "MiniTimerOne.h"
#include <stdio.h>
#include <stdlib.h>

#define greenLED1 11
#define greenLED2 10
#define greenLED3 6
#define greenLED4 5

#define button1 4
#define button2 7
#define button3 8
#define button4 12

#define potentiometer A0 
#define redLED 3

/* Costante da definire per la definizione del tempo */
const long k=1.2;

volatile uint8_t InterruptedPinShared;
volatile uint8_t PinStateShared;

unsigned char greenLEDs[4];
unsigned char buttons[4];

int currentLedOn, nextLedOn, temp, score, frequencyPot, level, levelGame, i, fadeValue;
boolean firstLedOn, checkCorrectClick, restartSystem, firstStart;
long gameTime, randomGameTime, tempInitialGameTime, microGameTime;
long time = 0;
boolean endTimer=false;

/* 
 *  
 *  La procedura setup serve ad inizializzare tutti i componenti e variabili
 *  necessarie per il corretto funzionamento del gioco. Vengono creati degli 
 *  array per memorizzare i quattro bottoni ed i quattro led verdi. Aumentiamo così
 *  la compattezza del programma.
 *  
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
  temp, score, i, fadeValue = 0;
  frequencyPot = 0;
  level, levelGame = 0;
  tempInitialGameTime, gameTime, microGameTime = 0;
  firstLedOn, checkCorrectClick = false;
  restartSystem, firstStart = true;  
  
  MiniTimer1.init();
  MiniTimer1.attachInterrupt(interruptTimer);
  
  Serial.begin(9600);
  Serial.println("Welcome to the Track to Led Fly Game. Press Key T1 to Start");

}

/*
 *  NB. Viene utilizzato nell'implementazione MiniTimer1. Questo ci permette di gestire
 *  il gameTime - tempo di gioco - in ogni fase dell'esercizio. 
 *  Questo non permette un tempo di attesa maggiore di 4 secondi. Per questo motivo,
 *  nel metodo getLevel() gli 8 livelli vengono suddivisi nei 4 secondi a disposizione.
 *  
 *  -------
 * 
 *  Nel superloop di arduino andiamo a sviluppare i comportamenti principali che
 *  quest'ultimo deve avere durante la sua esecuzione.
 *  
 *  FirstStart è una variabile boolena che controlla se è la prima volta che il sistema
 *  è avviato oppure è già stato in esecuzione durante il passato.
 *  Se il risultato è positivo, quindi è la prima volta che avviamo il programma, allora vengono
 *  rispettate le richieste della consegna, altrimenti procediamo con la normale esecuzione del gioco.
 *  
 *  restartSystem anche è una variabile booleana che ha l'onere di controllare se il bottone
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
 *  Timer1 tramite il metodo attachInterrupt() consente di terminare il gioco nel caso in cui
 *  il giocatore non prema nessun pulsante prima che il led si spenga. l'Interrupt viene chiamata 
 *  quando il Timer1 termina, quindi viene impostato sul tempo di gioco (gameTime).
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
    if (!(restartSystem == true)&&(endTimer==false)){
      digitalWrite(redLED, LOW);
      delay(10);
      for(int i=0; i<4; i++){
        digitalWrite(greenLEDs[i], LOW);
      }
      delay(10);
      static uint8_t InterruptedPin;
      static uint8_t PinState;
  
      currentLedOn = flashLed();

      noInterrupts();      
       InterruptedPin = InterruptedPinShared;
       PinState = PinStateShared;
      interrupts();

      while(endTimer == false){
        time = millis();
        fadeValue = 128+127*cos(2*PI/(gameTime*10)*(time));
        analogWrite(greenLEDs[currentLedOn], fadeValue);
        if(checkCorrectClick==true || restartSystem==true)
          break;
      } 
      
    }else{
  
      for(int i=0; i<4; i++){
        digitalWrite(greenLEDs[i], LOW);
      }

      Serial.print("Game Over - Score: ");
      Serial.println(score);
      score = 0;
      
      digitalWrite(redLED, HIGH);
          
      delay(2000);
      
      digitalWrite(redLED, LOW);
      restartSystem = false;

      MiniTimer1.stop();
      MiniTimer1.reset();
      MiniTimer1.setPeriod(gameTime*10000);
      MiniTimer1.start();

      
      firstStart = true;
      Serial.println("Welcome to the Track to Led Fly Game. Press Key T1 to Start");
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
    time = millis();
    fadeValue = 128+127*cos(2*PI/2000*time);
    analogWrite(redLED, fadeValue);
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
  long minGameTime = gameTime;
  long maxGameTime = (long)((gameTime*k)+1);
  gameTime = (minGameTime)+rand()%( maxGameTime - minGameTime);
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
 * Start al timer dal momento in cui premiamo - come codificato -
 * il primo pulsante tattile
 * 
 * 
*/

void incPunteggio(){
  
  InterruptedPinShared=arduinoInterruptedPin;
  PinStateShared=arduinoPinState;

  if(firstStart == true & buttons[0] == InterruptedPinShared){
    firstStart = false;
    Serial.println("GO!");
    getLevel();
    
    endTimer=false;
    MiniTimer1.stop();
    MiniTimer1.reset();
    MiniTimer1.setPeriod(gameTime*10000);
    MiniTimer1.start();
    
  }else{
    noInterrupts();
    for(int i=0; i<4; i++){ 
      if(buttons[i] == InterruptedPinShared && currentLedOn==i 
          && restartSystem == false && checkCorrectClick == false ){
            score++;
            Serial.print("Tracking the fly: pos ");
            Serial.println(currentLedOn);
            
            checkCorrectClick = true;
            
            gameTime = (gameTime/8)*7;            
            randomTime();

            MiniTimer1.stop();
            MiniTimer1.reset();
            MiniTimer1.setPeriod(gameTime*10000);
            MiniTimer1.start();
      }
    }

    if (checkCorrectClick == false){
      timesUp();
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
      gameTime = 400;
    break;
    
    case 129 ... 256:
      level = 2;
      gameTime = 350;
    break;
    
    case 257 ... 384:
      level = 3;
      gameTime = 300;
    break;
    
    case 385 ... 513:
      level = 4;
      gameTime = 250;
    break;

    case 514 ... 641:
      level = 5;
      gameTime = 200;
    break;

    case 642 ... 769:
      level = 6;
      gameTime = 150;
    break;

    case 770 ... 897:
      level = 7;
      gameTime = 125;
    break;

    case 898 ... 1023:
      level = 8;
      gameTime = 100;
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
 * Vengono utilizzati i metodi di MiniTimer1 stop e reset 
 * per gestire la nuova partita che - a breve - verrà riavviata.
 * 
*/

void timesUp(){
  restartSystem = true;
  MiniTimer1.stop();
  MiniTimer1.reset();
  MiniTimer1.setPeriod(gameTime*10000);
  MiniTimer1.start();
}

/*
 * 
 * Questa procedura, viene richiamata dall'interrupt handler del minitimer1.
 * Quando il tempo impostato da setPeriod() è passato, viene invocato questo metodo.
 * 
 * ----
 * 
 * Se endTimer è true nel loop non sarà permesso all'utente di giocare.
 * 
*/

void interruptTimer(void){
  endTimer = true;  
}
