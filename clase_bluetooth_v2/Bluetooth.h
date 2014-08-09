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
        void salir_modo_comando();
	void leer_data();
	void escribir_data();
        void buscar(char *data);
        void explorar();
	void menu();
        int editar_conexion(String mac);
        String cercano();
        int StringContains(String s, String search);
	String data;
	String mac;
        String mensaje;
};
#endif
