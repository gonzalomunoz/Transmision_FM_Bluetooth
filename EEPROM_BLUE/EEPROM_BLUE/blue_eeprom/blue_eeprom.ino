#include <EEPROM.h>

/*
  Direccion menor y Superior de
  memoria eeprom mega, cantidad maxima de
  equipos registrados
  tamaño que poseen las mac+caracter nulo 
  
*/
const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4096;
int EQUIPOS_REGISTRADOS=10;
const int BUFSIZE = 13;

/*
  Inicia EEPROM con valor 0xFF
*/
void eeprom_erase_all(byte b = 0xFF) {
  int i;
  for (i = EEPROM_MIN_ADDR; i <= EEPROM_MAX_ADDR; i++) {
    EEPROM.write(i, b);
  }
}


/*
  Returna true o false si la direccion 
  a revisar esta dentro del rango determinado
  por min y max 
*/
boolean eeprom_is_addr_ok(int addr) {
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}


/*
  Lee un string desde eeprom indicando
  su direccion de memoria y el tamaño a leer
*/
boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
  // byte read from eeprom
  byte ch;

  // number of bytes read so far
  int bytesRead;

  // check start address
  if (!eeprom_is_addr_ok(addr)) {
    return false;
  }

  // how can we store bytes in an empty buffer ?
  if (bufSize == 0) {
    return false;
  }

  // is there is room for the string terminator only,
  // no reason to go further
  if (bufSize == 1) {
    buffer[0] = 0;
    return true;
  }

  // initialize byte counter
  bytesRead = 0;

  // read next byte from eeprom
  ch = EEPROM.read(addr + bytesRead);

  // store it into the user buffer
  buffer[bytesRead] = ch;

  // increment byte counter
  bytesRead++;

  // stop conditions:
  // - the character just read is the string terminator one (0x00)
  // - we have filled the user buffer
  // - we have reached the last eeprom address
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
    // if no stop condition is met, read the next byte from eeprom
    ch = EEPROM.read(addr + bytesRead);

    // store it into the user buffer
    buffer[bytesRead] = ch;

    // increment byte counter
    bytesRead++;
  }

  // make sure the user buffer has a string terminator
  // (0x00) as its last byte
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }

  return true;
}

/*
  Escribe en eeprom el string enviado en una direccion en particular
*/
boolean eeprom_write_string(int addr, const char* string) {
  // actual number of bytes to be written
  int numBytes;

  // we'll need to write the string contents
  // plus the string terminator byte (0x00)
  numBytes = strlen(string) + 1;

  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

/*
  Escribe una secuencia de bytes en eeprom iniciando en una direccion especifica
*/
boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
  // counter
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range  
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }

  return true;
}
/*********FUNCIONES SOBRE MODULO BLUETOOTH***************/
String data="";
/*
Lectura de informacion proveniente de Serial1
*/
void leer_data(){
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
    delay(100);
}

void escribir_data(){
        Serial.println(data);
}

String obtener_mac_entrante(){
   char * pch;
   String mac_ent;
  int n=0;
  int cantidad_direcciones=9;
  int posicion_directorio=-1;
  //Serial.println(data);
  pch = strtok ((char*)data.c_str(),"\n,");
  while (pch != NULL){
    //Serial.println(pch);
    pch = strtok (NULL, ",\n");
    //Serial.println(pch);
    mac_ent=pch;
    pch = strtok (NULL, ",\n");
    pch = strtok (NULL, ",\n");
    pch = strtok (NULL, ",\n");
  }
  return mac_ent;
  
}
/*
  Setup
*/

String mac="12ADSC341299";

void setup(){
  
  Serial.begin(9600);
  Serial1.begin(9600);
  //LED ESTADO BORRADO DE EEPROM Y MAC RECONOCIDA
  pinMode(22,OUTPUT);
  //LED ESTADO TRANSMISION
  pinMode(23,OUTPUT);
  //BOTON BORRADO EEPROM
  pinMode(2,INPUT);
  char a=0;
  int i=0;
 // Serial.print("Borrar Mac Registradas\n [1] SI \nIngrese numero de Opcion: ");
  do{
      if(Serial.available()){
      a=Serial.read();
    }
    if(a=='1'||digitalRead(2)==HIGH){
      Serial.println(a);
      Serial.println("Borrando.....");
      digitalWrite(22,HIGH);
      eeprom_erase_all();
      digitalWrite(22,LOW);
      Serial.println("Mac Registradas Borradas");
      i=5;
    }
    i++;
    delay(2000);
  }while(i<5);
  delay(1000);
   mac="";
  delay(1000);
  Serial.println("\nEsperando Conexiones Entrantes");
}
String mac_entrante;
String mac_guardada;
 int i=0,b=0;
void loop(){
  //Lee informacion que provenga desde el dispositivo
  //Conexion
  data="";
  Serial1.flush();
    leer_data();
    if(data.length()>2){
      escribir_data();
      mac_entrante=obtener_mac_entrante();
      if(mac_entrante.length()==12){
            for(int u=0;u<EQUIPOS_REGISTRADOS;u++){
              eeprom_read_string(u*12,(char*)mac_guardada.c_str(), BUFSIZE);
              if(strcmp(mac_guardada.c_str(),(mac_entrante.c_str()))==0){
                b=1;
              }
            }
       if(b==0){
                //Leer key que venga del dispositivo
           data="";
            while(i<30){
              if (Serial1.available()){
                data+=(char)Serial1.read();
    
              if(data.length()>3){
                //escribir_data();
                if(data.compareTo("CONF")==0){
                  //Registrar Equipo
                  Serial.println("Sin Registro");
                  Serial.println("Registrando.....");
                  for(int u=1;u<4096;u++){
                        if(EEPROM.read(u)==255){
                           eeprom_write_string(u-1, mac_entrante.c_str());
                           u=5000;
                           EQUIPOS_REGISTRADOS++;
                        }
                    }
                   Serial.println("Registrado");
                   //
                  Serial.println("Enviando Musica");
                  Serial1.println("Enviando Musica");
                  digitalWrite(23,HIGH);
                  delay(5000);
                  digitalWrite(23,LOW);
                  Serial.println("Termino Envio Musica");
                  Serial1.println("Termino Envio Musica");
                  delay(1000);
                  /*Terminar Conexion*/
                  Serial1.print("$$$");
                  leer_data();
                  //escribir_data();
                  data="";

                  Serial1.println("K,1");
                  leer_data();
                 // escribir_data();
                  data="";
                }
                else{
                  Serial.println("Acceso Denegado");
                  Serial1.println("Acceso Denegado");
                  delay(1000);
                  Serial1.print("$$$");
                  leer_data();
                 // escribir_data();
                  data="";

                  Serial1.println("K,1");
                  leer_data();
                 // escribir_data();
                  data="";

                }
                i=50;
              }
             }
              i++;
              /*Time out*/
              if(i==30){
                Serial.println("Acceso Denegado");
                  Serial1.println("Acceso Denegado");
                  delay(1000);
                  Serial1.print("$$$");
                  leer_data();
                 // escribir_data();
                  data="";

                  Serial1.println("K,1");
                  leer_data();
                 // escribir_data();
                  data="";
              }
              delay(1000);
          }
        }
        if(b==1){
          /*Para Equipos registrados*/
          Serial.println("Equipo Registrado");
          Serial.println("Enviando Musica");
          Serial1.println("Enviando Musica");
          digitalWrite(22,HIGH);
          delay(5000);
          digitalWrite(22,LOW);
          Serial.println("Termino Envio Musica");
          Serial1.println("Termino Envio Musica");
          delay(1000);
          /*Terminar Conexion*/
          Serial1.print("$$$");
          leer_data();
           //escribir_data();
          data="";
      
          Serial1.println("K,1");
          leer_data();
          // escribir_data();
          data="";
          b=0;
        }        
        data="";
        i=0;
        
      }
       i=0;
      data="";
    }
}

