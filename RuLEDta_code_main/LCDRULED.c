/*#include "LCDRULED.h"

void mostrarPantallaLCD(String cadena,String cadena2){
lcd.clear();
lcd.setCursor(0,0);
lcd.print(cadena);
lcd.setCursor(0,1);
lcd.print(cadena2);
delay(2000);
}




void InicioLCD(){
lcd.setCursor(0,0); // This set the cursor in the position wanted, first paramenter indicates the column and the second the row. Typical max: 16x2.
lcd.print("La RuLEDta");
lcd.setCursor(0,1);
lcd.print("Insert COIN");
}


void mostrarPantallaLCDMOVIENDOSE(String cadena, String cadena2,int velocidad){
   // Obtenemos el tamaño del texto
  int tam_texto=cadena.length();
 
  // Mostramos entrada texto por la izquierda
  for(int i=tam_texto; i>0 ; i--)
  {
    String subcadena = cadena.substring(i-1); 
    // Limpiamos pantalla
    lcd.clear();
    //Situamos el cursor
    lcd.setCursor(0, 0);
    // Escribimos el texto
    lcd.print(subcadena);
    // Esperamos
    delay(velocidad);
  }
  // Desplazamos el texto hacia la derecha
  for(int i=1; i<=16;i++)
  {
    // Limpiamos pantalla
    lcd.clear();
    //Situamos el cursor
    lcd.setCursor(i, 0);
    // Escribimos el texto
    lcd.print(cadena);
    // Esperamos
    delay(velocidad);
  }
 
  // Desplazamos el texto hacia la izquierda en la segunda fila
  for(int i=16;i>=1;i--)
  {
    // Limpiamos pantalla
    lcd.clear();
    //Situamos el cursor
    lcd.setCursor(i, 1);
    // Escribimos el texto
    lcd.print(texto_fila);
    // Esperamos
    delay(VELOCIDAD);
  }
 
  // Mostramos salida del texto por la izquierda
  for(int i=1; i<=tam_texto ; i++)
  {
    String texto = texto_fila.substring(i-1);
    // Limpiamos pantalla
    lcd.clear();
    //Situamos el cursor
    lcd.setCursor(0, 1);
    // Escribimos el texto
    lcd.print(texto);
    // Esperamos
    delay(VELOCIDAD);
  }
}   
*/