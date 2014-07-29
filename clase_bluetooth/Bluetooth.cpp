#include "Bluetooth.h"

Bluetooth::Bluetooth(){
  data="";
  comando="";
  bandera=0;
}

/*
Funcion para ingresar el modo comando del dispositivo bluetooth
El dispositivo respondera con CMD para saber si ingreso al modo
de lo contrario arrojara ERR, con lo cual se vuelve a pedir ingreso
*/
void Bluetooth::modo_comando(){
  //Setear string data
    data="";
    //Aviso para conocer el paso donde esta
    Serial.print("\r\n-------------MODO COMANDO-------------*\n");
  do{
    // Serial.println("$$$");
    //Ingreso de comando para obtener respuesta del modulo
     Serial1.print("$$$");
     //Lectura de la informacion retornada por el modulo
     leer_data();
     //Escritura del retorno del modulo
     escribir_data();
     //Verificacion del retorno
     //Si retorna CMD, termina ciclo de lo contrario continua
  }while(data.compareTo("CMD")!=0);
    Serial.print("\r\n----------------FIN-------------------*\n");
}
/*
Funcion para obtener configuracion basica
Se ingresa comando D en modulo bluetooth
por medio de Serial1
El dispositivo retorna información, se leen cantidad maxima de 
caracteres 141
*/
void Bluetooth::leer_configuracion(){
    data="";
    /*Bandera Comando*/
    int a=1;
    /*Contar cantidad de Caracteres*/
    int b=0;
    Serial.print("\r\n-------------LEER CONF----------------*\n");
  do{
      //Ingreso comando mientras bandera este arriba
      if (a>0) {
        Serial1.println("D");
        //Se baja bandera
        a--;
      }
      //Si existe respuesta por parte del dispositivo
      //Se almacena respuesta
      if (Serial1.available()){
        byte in = Serial1.read();
        if((char)in!='?'){
          Serial.write(in);
          //Contar cantidad de caracteres maximo
          b++;
        }
      }
      //Si no existe respuesta se levanta bandera
      else{
        a=1;
      }
      /*131 Caracteres Configuracion Basica = D */
  }while(b<131);
    Serial.print("\r\n----------------FIN-------------------*\n");
}
/*
Funcion para salir del modo comando del dispositivo
Utiliza comando --- para salir
*/
void Bluetooth::salir_modo_comando(){
      data="";
     //Aviso de lo que se realizara
    Serial.print("\r\n----------SALIR MODO COMANDO----------*\n");
  do{
    // Serial.println("---");
    //Ejecucion comando de salida
     Serial1.println("---");
     //Leer respuesta de dispositivo
     leer_data();
     //Escribir respuesta de dispositivo
     escribir_data();
     //Si dispositivo responde con END se confirma salida
     //de lo contrario se realiza nuevamente la accion
  }while(data.compareTo("END")!=0);
    Serial.print("\r\n----------------FIN-------------------*\n");

}

void Bluetooth::ingresar_comando(){

}
/*
Funcion para configurar parametros de coneccion
Solicita MAC de equipo a conectarse
Utiliza codigo de pareo o pin de dispositivo
*/
void Bluetooth::realizar_conexion(){
}
/*
Lectura de informacion proveniente de Serial1
*/
void Bluetooth::leer_data(){
    byte data1;
    data="";
    while(Serial1.available()){
        data1=Serial1.read();
        //Omite caracteres de nueva linea y Carro
        if(((char)data1)!='\n'&&((char)data1)!='\r'){
          //Acumula los caracteres provenientes de Serial1
          data+=(char)data1;
        }
      }
    delay(1000);
}

void Bluetooth::escribir_data(){
        Serial.println(data);

}
/*
Funcion encargada de explorar dispositivos en un rango 
maximo de 20 mts
Utiliza comando I
*/
void Bluetooth::explorar(){
       data="";
       //Bandera para mantener ciclo
       int b=1;
      //Variable para cantidad de intentos
     //para ingresar comando I 
       int i=2;
    Serial.print("\r\n----------BUSCAR DISPOSITIVOS---------*\n");
  // Serial.println("---");
    while(b){
        if (Serial1.available()){
          byte d=Serial1.read();
          data+=(char)d;
          //Si respuesta de dispositivo esta completa contendra "Inquiry Done"
          //por ende se termina lectura de datos provenientes de Serial1
          //modificando bandera
          if(StringContains(data,"Inquiry Done")>0){
            b=0;
          }
         }
         //Cantidad de intentos para ingresar comando I
        if (i) {
          byte in = 'I';
          Serial.write(in); // local echo
          Serial1.println("I");
          //Retardo para esperar respuesta de comando ingresado
          //IMPORTANTE 
          delay(80);
          
        i--;  
      }
    }
    //Serial.println(data);
    /*Almacenar direcciones de los equipos*/
    char *direcciones;
    direcciones= strstr((char*)data.c_str(),"Found");
    Serial.print("Direcciones: ");Serial.println(direcciones);
    Serial.print("\r\n----------------FIN-------------------*\n");
    
}

void Bluetooth::menu(){
  
  if(Serial.available()){
     /*Lectura desde el puerto serial*/
   byte in= Serial.read();
    /*Ingresar a modo comando*/
    if((char)in=='C'){
      /*Si el modo comando ya se encuentra activado*/
      if(bandera==1){
          Serial.println("Ya se encuentra en modo Comando");
      }
      /*Activar el Modo Comando*/
      else{
          modo_comando();
          /*Si activa modo comando levanto bandera*/
          bandera=1;
        }
    }
    /*Solo si el modo comando esta activado*/
    if(bandera){
      /*Ingresar a lectura de configuracion basica*/
      if((char)in=='L'){
          leer_configuracion();
      }
      /*Ingresar a explorar equipos disponibles*/
      if((char)in=='E'){
          explorar();
      }
      /*Salir del Modo Comando*/
      if((char)in=='S'){
        salir_modo_comando();
        bandera=0;
      }
      /*Si algun caracter ingresado no esta dentro de las ordenes anteriores*/
      if((char)in!='C'&&(char)in!='L'&&(char)in!='E'&&(char)in!='S'&&(char)in!='M'&&(char)in!='T'){
        Serial.println("Orden No Encontrada");
      }
    }
    /*Ingresar al Menu*/
    if((char)in=='M'){
      Serial.println("Menu");
      Serial.println("C: Ingreso Modo Comando\n\t\t Acceso al Modo Comando del Dispositivo, para acceder a los demas comandos debe activar modo comando, ingrese C");
      Serial.println("L: Leer Configuracion\n\t\t Obtiene la Configuracion Basica del Dispositivo");
      Serial.println("E: Explorar los Equipos\n\t\tObtiene los equipos disponibles en un rango de 20 mts");
      Serial.println("S: Salir Modo Comando\n\t\t Secuencia de Escape del Modo Comando del Dispositivo");
      Serial.println("T: Enviar mensaje\n\t\t Enviar utilizando RX,TX\n\t\t [Importante: Debe estar conectado\n\t\t con algun equipo]");
      Serial.println("M: Menu\n\t\t Accede al Menu");
    }
      /*Si algun caracter ingresado no esta dentro de las ordenes anteriores*/
     if((char)in!='C'&&(char)in!='L'&&(char)in!='E'&&(char)in!='S'&&(char)in!='M'&&(char)in!='T'){
        Serial.println("Orden No Encontrada");
      }
   if((char)in=='T'){
     int b=1;
     while(b){
      if (Serial1.available())
        Serial.write(Serial1.read());
      if (Serial.available()) {
        byte in = Serial.read();
        Serial.write(in); // local echo
        Serial1.write(in);
        if(in=='*'){
          b=0;
        }
      }
     }
      
    }
  }
  leer_data();
  if(data.length()>2){
    escribir_data();
  }

}

/*
Funcion para realizar match de string contenido en otro String
Si encuentra String devuelve un valor mayor a 0
si no retorna un valor negativo
*/
int Bluetooth::StringContains(String s, String search) {
    int max = s.length() - search.length();
    int lgsearch = search.length();

    for (int i = 0; i <= max; i++) {
        if (s.substring(i, i + lgsearch) == search) return i;
    }

 return -1;
}
