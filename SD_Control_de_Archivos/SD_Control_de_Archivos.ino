// Biblioteca para el trabajo con tarjetas de memoria
#include <SD.h>

// Opciones del menú
int entrada;
int opcion = 0;

// Variables para la obtención de información de la tarjeta
Sd2Card card;
SdVolume volume;
SdFile root;

// Comunicación del Shield SD -> Pin 8 (Arduino UNO)
const int chipSelect = 8;

void setup() {
  // Inicialización de la comunicación serial
  Serial.begin(9600);
  
  // Pin de salida, estandar para el trabajo con el Shield SD
  pinMode(10, OUTPUT);
  
  // Revisa si hay una tarjeta y si puede inicializarse:
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("No hay tarjeta!");
    return;
  }
  else {
    SD.begin(chipSelect);
    Serial.println("SD inicializada correctamente.");
    // Se levanta el menú de opciones
    menu();
  }
}

void loop() {
  // Retorno al menú de opciones
  if (Serial.available()){
    entrada=Serial.read();
    if(entrada=='*'){
      menu();
    }    
  }
}

/* 
  Esta función crea un menú de interacción
  por medio de la comunicación serial.
*/
void menu(){
  int entradaMenu;
  Serial.println("MENU PRINCIPAL:");
  Serial.println("(1) Ver informacion de la tarjeta");
  Serial.println("(2) Agregar un archivo");
  Serial.println("(3) Ver el contenido de un archivo");
  Serial.println("(4) Borrar un archivo");
  
  do{
  }while(Serial.available()==0);
  
  entradaMenu=Serial.read();
  switch(entradaMenu){ 
    case '1':
      infoTarjeta();
      delay(1000);
      opcion = 1;
      break;
      
    case '2':
      agregarArchivo();
      delay(1000);
      opcion = 2;
      break;

    case '3':
      verArchivo();
      delay(1000);
      opcion = 3;
      break;
    
    case '4':
      eliminarArchivo();
      delay(1000);
      opcion = 4;
      break;

    default:
      Serial.println("");
      Serial.println("La opción ingresada no es válida!");
      opcion = 0;
      break;
    }
    
    if (opcion == 1 || opcion == 2 || opcion == 3 || opcion == 4)
    Serial.println("");
    Serial.println("--> Presiona (*) para volver al menu");
}

/*
  Función encargada de eliminar
  un archivo de la tarjeta de memoria
  especificado.
*/
void eliminarArchivo() {
  String nombre_archivo = "";
  
  Serial.println();
  Serial.print("Ingrese el nombre del archivo: ");
  do {
    nombre_archivo = Serial.readString();
  }
  while (nombre_archivo == "");
  Serial.println(nombre_archivo);  

  // Convertir el nombre del archivo a char*
  char file_name[nombre_archivo.length()+1];
  nombre_archivo.toCharArray(file_name, sizeof(file_name));
  
  if(SD.exists(file_name)) {
    SD.remove(file_name);
    Serial.print("El archivo <");
    Serial.print(file_name);
    Serial.println("> fue eliminado.");
  }
  else {
    Serial.print("El archivo <");
    Serial.print(file_name);
    Serial.println("> no existe.");
  }
}

/*
  Función encargada de ver la información
  contenida en un archivo de texto.
*/
void verArchivo() {
  String nombre_archivo = "";
  
  Serial.println();
  Serial.print("Ingrese el nombre del archivo: ");
  do {
    nombre_archivo = Serial.readString();
  }
  while (nombre_archivo == "");
  Serial.println(nombre_archivo);  

  // Convertir el nombre del archivo a char*
  char file_name[nombre_archivo.length()+1];
  nombre_archivo.toCharArray(file_name, sizeof(file_name));
  
  if(SD.exists(file_name)) {
    File archivo = SD.open(file_name, FILE_READ);  
    if (archivo) {
      Serial.println("En contenido del archivo es: ");
      
      String contenido = "";
      while (archivo.available()) {
          contenido += (char)archivo.read();
      }
      archivo.close();
      Serial.println(contenido);
    } else {
      Serial.println("Error al abrir el archivo.");
    }
  }
  else {
    Serial.print("El archivo <");
    Serial.print(file_name);
    Serial.println("> no existe.");
  }
}

/*
  Función encargada de crear un archivo 
  (junto con su contenido) y almacenarlo
  en la tarjeta de memoria.
*/
void agregarArchivo() {

  String nombre_archivo = "";
  String contenido_archivo = "";
  
  Serial.println();
  Serial.print("Ingrese el nombre del archivo: ");
  do {
    nombre_archivo = Serial.readString();
  }
  while (nombre_archivo == "");
  Serial.println(nombre_archivo);
  
  Serial.println("Ingrese el contenido del archivo: ");
  do {
    contenido_archivo = Serial.readString();  
  }
  while (contenido_archivo == "");
  Serial.println(contenido_archivo);
  
  Serial.println();
  
  // Convertir el nombre del archivo a char*
  char file_name[nombre_archivo.length()+1];
  nombre_archivo.toCharArray(file_name, sizeof(file_name));
  
  //Serial.println(file_name);
  File archivo = SD.open(file_name, FILE_WRITE);
  if (archivo) {
    archivo.println(contenido_archivo);
    archivo.close();
    Serial.println("Archivo creado correctamente.");
  }  
  else {
    Serial.println("Error al crear el archivo.");
  }
}

/*
  Función encargada de mostrar la información
  de la tarjeta de memoria, además de la lista
  de archivos contenidos en ella.
*/
void infoTarjeta() {
  // Inprime el tipo de tarjeta
  Serial.print("\nTipo de tarjeta: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

 // Se revisa la partición de la tarjeta (debe ser FAT16 o FAT32)
  if (!volume.init(card)) {
    Serial.println("No se encuentra una partición FAT16 o FAT32.");
    return;
  }
  
  // Se imprime el tipo y el tamaño de la partición
  uint32_t volumesize;
  Serial.print("El tipo de volumen es FAT");
  Serial.println(volume.fatType(), DEC);
  
  volumesize = volume.blocksPerCluster();
  volumesize *= volume.clusterCount();
  volumesize *= 512;
  volumesize /= 1024;
  volumesize /= 1024;
  
  Serial.print("Su tamano total es: ");
  Serial.print(volumesize);
  Serial.println(" Mb.");
  
  Serial.println("\nLos archivos encontrados en la tarjeta (nombre, fecha y tamano en [bytes]) son: ");
  root.openRoot(volume);
  
  // Lista todos los archivos de la tarjeta incloyendo el nombre, la fecha y el tamano en bytes
  root.ls(LS_R | LS_DATE | LS_SIZE);
}


