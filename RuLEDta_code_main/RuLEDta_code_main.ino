/*-----------------------------------------------------------------------------------------------------*/
//                                        RULEDTA ARDUINO CODE
//                                CAREFUL WITH THE BETS AND HAVE FUN!
// In this main code, there no implementation of LCD screen, if you want to use it check the instructable
// and the LCD.h and LCD.c attached
//
//
// RuLEDta (LED Betting Roulette) © 2024 by Francisco Javier Baena and Ismael Carmona is licensed under CC BY-NC 4.0 
/*-----------------------------------------------------------------------------------------------------*/


#include <Adafruit_NeoPixel.h>
#include <Keypad.h>
#ifdef _AVR_
#include <avr/power.h>
#endif

//libraries

#include <Wire.h>

#include "LCDRULED.h"

#include "Button.h"
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
void printDetail(uint8_t type, int value);
#define APUESTA_ROJO -1
#define APUESTA_NEGRO -2
#define APOSTADO 200
#define NO_APOSTADO 0
//LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display . Uncomment if using a LCD screen.
#define PIN 2
#define NUMLEDS 37
#define ERROR 80
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);
const byte rowsCount = 4;
const byte columsCount = 4;

int array_apuestas[NUMLEDS];

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

char keys[rowsCount][columsCount] = {
{ '1', '2', '3', 'R' },
{ '4', '5', '6', 'N' },
{ '7', '8', '9', 'C' },
{ '*', '0', '#', 'D' }
};

const  int pin_modena = 13;
String apuestas_totales = "";

Button detector_moneda(pin_modena);
// Keypad must be connected to D11~D4 arduino pins. If you want to change those, change the next lines too.
const byte rowPins[rowsCount] = { 11, 10, 9, 8 };
const byte columnPins[columsCount] = { 7, 6, 5, 4 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);

int ledValues[NUMLEDS] = { 0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30, 8, 23, 10, 5, 24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29, 7, 28, 12, 35, 3, 26 }; // Value of the positions of the RuLEDta Sectors.
int winningValue = 0;
int semaforo;
void rellenar_array_apuestas(int valor);


void setup() { // SETUP!!!
      FPSerial.begin(9600);
      //Serial.begin(115200); // Enable if you want to get the comments in your computer`s serial port.
      if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
          Serial.println(F("Unable to begin:"));
          Serial.println(F("1.Please recheck the connection!"));
          Serial.println(F("2.Please insert the SD card!"));
          while(true);
      }

      
      myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
      
      myDFPlayer.volume(25);  // Volume value [0~30]
      myDFPlayer.volumeUp(); //Volume Up
      myDFPlayer.volumeDown(); //Volume Down
      myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
      myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);


    //----- Initialize the Neopixel strip with a general brightness and establish the roulette's coulours.
    //----- Also we start playing the ambiental music
    myDFPlayer.playMp3Folder(1);
    strip.begin();
    strip.setBrightness(60);
    strip.show(); // Initialize all pixels to 'off'
    randomSeed(analogRead(1));

    // Turn on Neopixeles Green Red and Blue for '0'
    InicioRuleta();
}

// Turn on Neopixeles Green Red and Blue for '0'
void InicioRuleta() {

    for (int i = 0; i < NUMLEDS; i++) {
        if (i == 0) {
        strip.setPixelColor(i, strip.Color(0, 0, 250)); // Blue for '0'
        } else if (i % 2 == 0) {
        strip.setPixelColor(i, strip.Color(250, 0, 0)); // Red for even numbers
        } else { 
        strip.setPixelColor(i, strip.Color(0, 250, 0)); // Green for odd numbers
        }
    }

    strip.show();
}


int Apostar_individual(){
  String cadenitaLCD1,cadenitaLCD2;
  char key = keypad.getKey();
  int apuestaNumerica = 0;
  cadenitaLCD1 = String(key);
  //Data will be read until # is pressed. 
  //mostrarPantallaLCD("pulse # para terminar de apostar",apuestas_totales);
  //while(key != '#'){
  while(key != '*'){
    //cadenitaLCD2 = String(key);
    //strcat(cadenitaLCD1,cadenitaLCD2);
    key = keypad.getKey();
    cadenitaLCD2 = String(key);
    cadenitaLCD1 += cadenitaLCD2;
    }
    apuestaNumerica = cadenitaLCD1.toInt();
    //Serial.println(apuestaNumerica);
    cadenitaLCD1[cadenitaLCD1.length()-1] = ' ';
    if(key){

        if(cadenitaLCD1 == "0 "){
          //mostrarPantallaLCD("Apuesta " + cadenitaLCD1, apuestas_totales);
          rellenar_array_apuestas(apuestaNumerica);
           Serial.println("apuesta al:");
           Serial.println(apuestaNumerica);
           delay(2000);
          apuestaNumerica = 0;
        }else if(cadenitaLCD1 == "R "){
          //mostrarPantallaLCD("Apuesta a rojo", apuestas_totales);
          for (int i = 1; i < NUMLEDS; i++) {
              if (i % 2 == 0) {
              rellenar_array_apuestas(ledValues[i]);  
              }
          }
          delay(2000);
          apuestaNumerica = APUESTA_ROJO;
        }else if(cadenitaLCD1 == "N "){
          //mostrarPantallaLCD("Apuesta a negro",apuestas_totales);
           for (int i = 1; i < NUMLEDS; i++) {
              if (i % 2 != 0) {
              rellenar_array_apuestas(ledValues[i]);    
              }                        
          }
          delay(2000);
          apuestaNumerica = APUESTA_NEGRO;
        }else if (cadenitaLCD1 == "D1 ") {
            //mostrarPantallaLCD("Apuesta a docena 1", apuestas_totales);
            for(int i = 1; i <= 12;i++){    
                rellenar_array_apuestas(i);           
            }
        }else if(cadenitaLCD1== "D2 "){
          for(int i = 13; i <= 24;i++){    
                rellenar_array_apuestas(i);           
            }

              //mostrarPantallaLCD("Apuesta a docena 2", apuestas_totales);
        }else if(cadenitaLCD1 =="D3 "){
          for(int i = 25; i <= 36;i++){    
                rellenar_array_apuestas(i);           
            }
              //mostrarPantallaLCD("Apuesta a docena 3", apuestas_totales);
        }else if(apuestaNumerica > 37 || apuestaNumerica == 111){
              myDFPlayer.playMp3Folder(11);
              apuestaNumerica = ERROR;      
        }else if(apuestaNumerica > 37 || apuestaNumerica == 112){
              myDFPlayer.playMp3Folder(12);
              apuestaNumerica = ERROR; 
        }else if(apuestaNumerica > 37 || apuestaNumerica == 113){
              myDFPlayer.playMp3Folder(13);
              apuestaNumerica = ERROR;
        }else if(apuestaNumerica > 37 || apuestaNumerica == 114){
              myDFPlayer.playMp3Folder(14);
              apuestaNumerica = ERROR;
        }else if(apuestaNumerica > 37 || apuestaNumerica == 0){
          //mostrarPantallaLCD("Apuesta incorrecta",";V");
          apuestaNumerica = ERROR;
        }else{
          //mostrarPantallaLCD("Apuesta "+ cadenitaLCD1,apuestas_totales);
           rellenar_array_apuestas(apuestaNumerica);
           Serial.println("apuesta al:");
           Serial.println(apuestaNumerica);
           delay(2000);
        }
       


    }

 // }
  
return apuestaNumerica;
}
int Apuesta_multiple(){
   String cadenitaLCD1,cadenitaLCD2;
  char key = keypad.getKey();
  cadenitaLCD1 = String(key);
  int numero_apuestas= 0;
  //Data will be read until # is pressed. 
  //mostrarPantallaLCD("pulse # para terminar de apostar",apuestas_totales);
  //while(key != '#'){
  while(key != '#'){
    //cadenitaLCD2 = String(key);
    //strcat(cadenitaLCD1,cadenitaLCD2);
    key = keypad.getKey();
    cadenitaLCD2 = String(key);
    cadenitaLCD1 += cadenitaLCD2;
    }
    numero_apuestas = cadenitaLCD1.toInt();
    if(numero_apuestas > 10){
      //no hacemos nada
      numero_apuestas = 0;
      Serial.println("demasiadas apuestas ya!");
    }else{
      //Serial.println(apuestaNumerica);
      cadenitaLCD1[cadenitaLCD1.length()-1] = ' ';
      int valor;
      Serial.println("Numero_apuestas:");
      Serial.println(numero_apuestas);
      //mostrarPantallaLCD("pulse # para terminar de apostar",apuestas_totales);
      for(int x = 0; x < numero_apuestas;x++){
      valor = Apostar_individual();
    
      }

    }
    
  return numero_apuestas;
}


void rellenar_array_apuestas(int valor){
    bool salir = true;
    int cont = 0;
    while(salir && cont <= 36){
          if(valor == ledValues[cont]){
            salir = false;
            array_apuestas[cont] = APOSTADO;
            strip.setPixelColor(cont, strip.Color(0, 0, 0));    
            strip.show();  
          }
          cont++;
  }

}

void limpiar_array_apuestas(){
  for(int i = 0; i < NUMLEDS;i++){
    array_apuestas[i] = NO_APOSTADO;
  }
}

void comprobar() {
  bool premio = false;
  for(unsigned x = 0; x < NUMLEDS;x++){
    if(array_apuestas[x] == APOSTADO){
      if(x == winningValue){
        premio = true;
      }
    }
  }
  if(premio){
    Serial.println("Enhorabuenaaa!!");
    myDFPlayer.playMp3Folder(5); //Victory sound
    ganar();
    
  }else{
    myDFPlayer.playMp3Folder(6);//Defeat sound
    Serial.println("NO ha habido suerte, lo único que la máquina está calentica! Hasta pronto!");
  }
  premio = false;
}


void colorWipe(uint32_t c, uint32_t tardansa, uint32_t decremento, long totalsaltos) {
    uint32_t espera;
    //uncomment this line to start the oscillation in a random number, in this code we are starting on 0 to make the effect more striking
    //long ledEncendido = random(0, NUMLEDS);
    long ledEncendido = 0;
    long ledactual = ledEncendido;
    int vueltasRealizadas = 0;
    espera = tardansa;


    while (ledEncendido <= totalsaltos) {
        //Serial.print("ledEncendido: ");
        //Serial.println(ledEncendido);
        if ((ledEncendido % NUMLEDS) == 0) {
        ledactual = 0;
        espera += 50;
        }

        //Serial.print("ledactual: ");
        //Serial.println(ledactual);
        strip.setPixelColor(ledactual, c);
        strip.show();
        delay(espera);
        //strip.setPixelColor(ledactual, strip.Color(0, 0, 0));
        if (ledactual == 0) {
            strip.setPixelColor(ledactual, strip.Color(0, 0, 250)); //Azul
        } else if (ledactual % 2 == 0) {
            strip.setPixelColor(ledactual, strip.Color(250, 0, 0)); // Rojo
        } else {
            strip.setPixelColor(ledactual, strip.Color(0, 250, 0)); // Verde
        }

        ledEncendido++;
        ledactual++;

    }
    //while(1);
    ledfinal(ledactual);


}

void asignarvalorleds() {
    for (int i = 0; i < NUMLEDS; i++) {
    int value = ledValues[i];
    }
}


void ledfinal(uint8_t pos) {
    uint8_t cont = 0;
    winningValue = pos;
    while (++cont < 10) {
      strip.setPixelColor(pos, strip.Color(250, 250, 250));
      strip.show();
      delay(50);
      strip.setPixelColor(pos, strip.Color(0, 0, 250));
      strip.show();
      delay(50);
      strip.setPixelColor(pos, strip.Color(250, 0, 0));
      strip.show();
      delay(50);
      strip.setPixelColor(pos, strip.Color(0, 250, 0));
      strip.show();
      delay(50);
    }

    InicioRuleta();

}


void ganar(){

for (int ix = 0; ix < 4; ix++) {
    
  for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, strip.Color(201, 125, 250));
    }
    strip.show();
    delay(1000);
    for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 120, 120));

    }
    strip.show();
    delay(1000);
    for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, strip.Color(150, 100, 250));

    }
    strip.show();
    delay(1000);
    }
    //InicioRuleta();

}

void apagar() {
    for (uint16_t i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}


uint32_t Wheel(byte WheelPos) {
      WheelPos = 255 - WheelPos;
      if(WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
      }

      if(WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      WheelPos -= 170;
      return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void rainbowCycle(uint8_t wait) {
      uint16_t i, j;

      for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
        for(i=0; i< strip.numPixels(); i++) {
          strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }
        strip.show();
        delay(wait);
      }
    }


    int monedas_necesarias = 0;
    int monedas_actuales = 0;

void loop() {
    long totalsaltos = random(NUMLEDS * 3, NUMLEDS * 4); // se busca una posición aleatoria a la que llegar


    if(detector_moneda.isPressed()){
      if(!semaforo){
        
          if(monedas_necesarias == 0){
              myDFPlayer.playMp3Folder(2); //audio ¡hagan sus apuestas!
              delay(1000);
              monedas_necesarias = Apuesta_multiple();
              Serial.println("se van a apostar");
              Serial.println(monedas_necesarias);
              myDFPlayer.playMp3Folder(3); // Please insert coin
          }
          monedas_actuales++;   
          Serial.println(monedas_actuales);
          semaforo = true;
          if(monedas_actuales == monedas_necesarias){
            myDFPlayer.playMp3Folder(4); //Epic music
            // Init of the oscillation of the roulette.
            colorWipe(strip.Color(200, 200, 200), 50, 2, totalsaltos);
            //checking the bets
            comprobar();      
            // Idle state  
            rainbowCycle(5);
            // Idle music
            myDFPlayer.playMp3Folder(1); //audio idle music
            monedas_actuales = 0;
            monedas_necesarias = 0;

              // We have to clean the betting array for the next iteration
              limpiar_array_apuestas();
              // If you want to set the roulette's default colors uncomment the following line
              //InicioRuleta(); 
          }

      }

    }else{
        semaforo = false;
    }

}


/*-----------------------------------------------------------------------------------------------------*/
//                                        RULEDTA ARDUINO CODE
//                           
//
//RuLEDta (LED Betting Roulette) © 2024 by Francisco Javier Baena and Ismael Carmona is licensed under CC BY-NC 4.0 
/*-----------------------------------------------------------------------------------------------------*/
