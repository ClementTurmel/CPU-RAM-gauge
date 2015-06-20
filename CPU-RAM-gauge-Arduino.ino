//*************************
//** Name : CPU-RAM-gauge
//** Author : Clemaul
//** Version : 2.0
//** date : 20150620
//*************************

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24

const int myButton = 5;
int myButton_state;
int myButton_memory = HIGH;
//int etat;
int mode = 0; // 0 -> ALL , 1 ->  CPU , 2 -> RAM

int byte_read = 0; ///< The current byte
int recieved_integer = 0; ///< The current recieved integer
int recieved_value = 0;
String recieved_string = "";



// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 200; // delay for half a second

  void setup() {
    Serial.print("PROGRAM START");
    pixels.begin(); // This initializes the NeoPixel library.
    //Serial.begin(9600);
    Serial.begin(115200); // speed correcting synchronization problem
    pinMode(myButton, INPUT);
    pinMode(myButton, INPUT_PULLUP);
  }

  //affiche les le pourcentage converti en nombre de led sur le segment gauche.
  //display percents cenvert to number of leds on the left segment.
  void printLeftSegment( int nbLed ){
    
    for(int i=0;i<nbLed ;i++){
      if(i == 5 || i == 11){
        pixels.setPixelColor(i, pixels.Color(5,5,5));
      }
      else{
        pixels.setPixelColor(i, pixels.Color(0,0,5));
      }
    }
  }
  
  //affiche les le pourcentage converti en nombre de led sur le segment droit.
  void printRightSegment( int nbLed ){

    for(int j=(NUMPIXELS-1);j>((NUMPIXELS-1)-nbLed);j--){  
      if(j == 12 || j == 18){
        pixels.setPixelColor(j, pixels.Color(5,5,5));
      }
      else{
        pixels.setPixelColor(j, pixels.Color(0,5,0));
      }
    }
  }
  
  void  printAllCircle(int nbLed , int Red , int Green , int Blue ){
      for(int i=0;i<nbLed ;i++){
      if(i == 5 || i == 11 || i == 17 ||i ==23){
        pixels.setPixelColor(i, pixels.Color(5,5,5)); // marker at 25,50,75 and 100%  
      } 
      else{
        pixels.setPixelColor(i, pixels.Color( Red , Green , Blue )); 
      } 
    }
  }

  //convert percent to number leds before sending it to coresponding segment.
  //transforme le pourcentage en nombre de led avant d'etre envoyé au segment corespondant.
  //int segment : represent the segment to show pourcentage
  //float pourcent : represent thhe pourcentage to show
  void printStat( int segment , float pourcent){
    
   Serial.print("segment :");
    Serial.println(segment);
    
    int diviseur = 2; // DEFAULT 
   /* if(segment == 2 || segment == 3){
      diviseur = 1; // ALL CIRCLE need all leds
    }*/
    
    switch(segment){
      case 2:
      {
        diviseur = 1;
      }
      case 3:
      {
        diviseur = 1; 
      }
      default:
      {
        diviseur = 2; 
      }
    
    }
       
    Serial.print("Diviseur :");
    Serial.println(diviseur);
    
    float nbLed = ( pourcent * (24.00 / diviseur) ) / 100.00; // convert pourcent to led
    /*Serial.print("Valeur avant arrondi :");
    Serial.println(nbLed);*/
    
    nbLed = nbLed + 0.50; // rounded to up (ex : 4.6 -> 5.1)
    /*Serial.print("Valeur APRES arrondi :");
    Serial.println(nbLed); // trunked to unity (ex 5.1 -> 5)*/
    
    int trunkedLed = (int)(nbLed);
    /*Serial.print("Valeur APRES TRONCKAGE :");
    Serial.println(trunkedLed);*/
    
    switch(segment){
      case 0: // LEFT
      {
        printLeftSegment(trunkedLed);
        break;
      }
      case 1: // RIGHT
      {
        printRightSegment(trunkedLed);
        break;
      }
      case 2: // ALL CIRCLE CPU
      {
        printAllCircle(trunkedLed , 0 , 0 , 5); // BLUE
        break;
      }
      case 3: // ALL CIRCLE RAM
      {
        printAllCircle(trunkedLed , 0 , 5 , 0); // GREEN
        break;
      }
    }

  }

  //reset all led
  //réinitialise l'ensemble des leds (OFF)
  void cleanLED()
  {
    for(int leds = 0 ; leds <24 ; leds++)
    {
      pixels.setPixelColor(leds, pixels.Color(0,0,0));
    }
  }

  //Define is the integer recieve is a number
  //Determine si l'entier ascii recu est est nombre
  boolean is_a_number(int n)
  {
    return n >= 48 && n <= 57;
  }
  
  // convertion en réel de l'ascii vers l'entier
  int ascii2int(int n, int byte_read)
  {
    return n*10 + (byte_read - 48);
  }

  //Analyze recieved integer, initiate percents and chose display mode
  //Analyse l'entier reçu, initialise les pourcentage et choisi le mode d'affichage
  void setPercents(int recieved_integer){
  
    //--------------------12345
    //integer is formed : 1XXYY
    // 1 the start of the message
    // 2 and 3 represent the CPU percent
    // 4 and 5 represent the RAM percent
    
    recieved_string = (String)recieved_integer;
    
    int CPU = (recieved_string.substring(1,3)).toInt();
    int RAM = (recieved_string.substring(3,5)).toInt();
    
    Serial.print("CPU -> ");
    Serial.print(CPU);
    Serial.print("  RAM -> ");
    Serial.println(RAM);
    
    //LISTEN BUTTON
    myButton_state = digitalRead(myButton); // LISTEN BUTTON STATE
    
    //Si le bouton a un état différent que celui enregistré ET que cet état est "appuyé"
    //if the button have a different state that the one register and if this one is press
    if((myButton_state != myButton_memory) && (myButton_state == LOW))
    {
        mode = mode + 1;
        if(mode == 3){
          mode = 0;
        }
    }
    
    //on enregistre l'état du bouton pour le tour suivant
    //Whe save stat for next time
    myButton_memory = myButton_state; //on enregistre l'état du bouton pour le tour suivant
    
    
    switch(mode){
     case 0:
     {
       printStat( 0 , CPU); 
       printStat( 1 , RAM);
       break;
     }
     case 1:
     {
       printStat( 2 , CPU);
       break;
     } 
     case 2:
     {
       printStat( 3 , RAM);
       break;
     }
    }

    Serial.print("MODE -> ");
    Serial.println(mode);
    
  }

  void loop() {
    
    cleanLED();
  
    recieved_integer = 0;
    
    while ( Serial.available() ) {
      byte_read = Serial.read();
      if ( is_a_number(byte_read) ) {   
            recieved_integer = ascii2int(recieved_integer, byte_read);
            //Serial.println("entier recu : ");
            //Serial.println( recieved_integer );
      }
      
      if( recieved_integer >= 10000 && recieved_integer <= 19999 ){
        recieved_value = recieved_integer;
      } 
    }
  
    setPercents(recieved_value);
    
    pixels.show();
    //delay(500);
    delay(250);
  }
