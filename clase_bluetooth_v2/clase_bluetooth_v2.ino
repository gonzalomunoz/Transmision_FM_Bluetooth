/*
Programa creado para realizar exploracion con modulo de comunicacion bluetooth RN42
Autor: Gonzalo Muñoz
Fecha: 25-07-2014
*/
/*
  SA, Encriptacion
  SM, Modo Operacion
  I, Explorar zona
  SX, bonding
  SU,Badraute
  SN,nombre_equipo
  
*/

#include "Bluetooth.h"

Bluetooth bt;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
 // Serial.println("Inicio Detector Bluetooth");
 // Serial.println("Ingrese M");
 delay(10000);
}

void loop(){
  
  bt.menu();
}
