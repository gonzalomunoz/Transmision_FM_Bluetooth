/*Libreria para Bluetooth
Autor: Gonzalo Muñoz
*/
#include "Arduino.h"

#ifndef Bluetooth_h 
#define Bluetooth_h 
class Bluetooth{
 public:
	Bluetooth();
	void modo_comando();
        void leer_configuracion();
        void salir_modo_comando();
	void ingresar_comando();
	void realizar_conexion();
	void leer_data();
	void escribir_data();
        void explorar();
	void menu();
        int StringContains(String s, String search);
	String data;
	String comando;
      String mensaje;
        int bandera;
};
#endif
