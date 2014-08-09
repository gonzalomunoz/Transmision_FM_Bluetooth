#include <EEPROM.h>


//
// Absolute min and max eeprom addresses.
// Actual values are hardware-dependent.
//
// These values can be changed e.g. to protect
// eeprom cells outside this range.
//
const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4096;
int EQUIPOS_REGISTRADOS=10;
const int BUFSIZE = 13;

//
// Initialize eeprom memory with
// the specified byte.
// Default value is 0xFF.
//
void eeprom_erase_all(byte b = 0xFF) {
  int i;

  for (i = EEPROM_MIN_ADDR; i <= EEPROM_MAX_ADDR; i++) {
    EEPROM.write(i, b);
  }
}


//
// Returns true if the address is between the
// minimum and maximum allowed values,
// false otherwise.
//
// This function is used by the other, higher-level functions
// to prevent bugs and runtime errors due to invalid addresses.
//
boolean eeprom_is_addr_ok(int addr) {
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}


//
// Reads a string starting from the specified address.
// Returns true if at least one byte (even only the
// string terminator one) is read.
// Returns false if the start address falls outside
// or declare buffer size os zero.
// the allowed range.
// The reading might stop for several reasons:
// - no more space in the provided buffer
// - last eeprom address reached
// - string terminator byte (0x00) encountered.
// The last condition is what should normally occur.
//
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
//
// Writes a string starting at the specified address.
// Returns true if the whole string is successfully written.
// Returns false if the address of one or more bytes
// fall outside the allowed range.
// If false is returned, nothing gets written to the eeprom.
//
boolean eeprom_write_string(int addr, const char* string) {
  // actual number of bytes to be written
  int numBytes;

  // we'll need to write the string contents
  // plus the string terminator byte (0x00)
  numBytes = strlen(string) + 1;

  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}
//
// Writes a sequence of bytes to eeprom starting at the specified address.
// Returns true if the whole array is successfully written.
// Returns false if the start or end addresses aren't between
// the minimum and maximum allowed values.
// When returning false, nothing gets written to eeprom.
//
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
//
// SETUP
//

String mac="12ADSC341299";
void setup() {

  Serial.begin(9600);
  char a=0;
  int i=0;
  Serial.print("Borrar Mac Registradas\n [1] SI \n [0] NO \nIngrese numero de Opcion: ");
  do{
      if(Serial.available()){
      a=Serial.read();
    }
    if(a=='1'){
      Serial.println(a);
      Serial.println("Borrando.....");
      eeprom_erase_all();
      Serial.println("Mac Registradas Borradas");
      i=5;
    }
    i++;
    delay(2000);
  }while(i<5);
  delay(1000);
  if(a=='0'){
    Serial.println(a);
  }
   mac="";
  delay(1000);
  Serial.println("\nEsperando Conexiones Entrantes");
}


//
// LOOP
//
int a=0,b=0;
String mac_guardada;
/*Mac a verificar si esta guardada*/
void loop() {
  if(Serial.available()){
    char c=Serial.read();
    if(c!='\n' && c!='\r'){
      mac+=c;
    }
  }
  if(mac.length()==12){
    a=1;
    Serial.println(mac);
  }
  if(a){
    for(int u=0;u<EQUIPOS_REGISTRADOS;u++){
      eeprom_read_string(u*12,(char*)mac_guardada.c_str(), BUFSIZE);
      if(strcmp(mac_guardada.c_str(),(mac.c_str()))==0){
        b=1;
      }
    }
    a=0;
    if(b==1){
      Serial.println("MAC ENCONTRADA");
      b=0;
    }
    else{
      Serial.println("MAC NO ENCONTRADA");
      /*Recibir KEY para registrar mac*/
      /*Si recibe key*/
        for(int u=1;u<4096;u++){
            if(EEPROM.read(u)==255){
               eeprom_write_string(u-1, mac.c_str());
               u=5000;
               EQUIPOS_REGISTRADOS++;
            }
        }
      b=0;
    }
      mac="";
      mac_guardada="";
   }
}
