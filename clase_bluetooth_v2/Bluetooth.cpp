#include "Bluetooth.h"
#define pin_explora 22
/*TABLA DE DIRECCIONES */
//Maximo 10 direcciones
String direccion[10]={"2CD05A9BB7FC","CC52AF727611","34BB26B8CC88"};
String mac_cercana;
int encontradas[10];
int rssi[10];
Bluetooth::Bluetooth(){
  data="";
  mac="";
  pinMode(pin_explora,OUTPUT);
  
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
   // Serial.print("\r\n-------------MODO COMANDO-------------*\n");
  do{
    // Serial.println("$$$");
    //Ingreso de comando para obtener respuesta del modulo
     Serial1.print("$$$");
     //Lectura de la informacion retornada por el modulo
     leer_data();
     //Escritura del retorno del modulo
     //escribir_data();
     //Verificacion del retorno
     //Si retorna CMD, termina ciclo de lo contrario continua
  }while(data.compareTo("CMD")!=0);
   // Serial.print("\r\n----------------FIN-------------------*\n");
}

/*
Funcion para salir del modo comando del dispositivo
Utiliza comando --- para salir
*/
void Bluetooth::salir_modo_comando(){
      data="";
     //Aviso de lo que se realizara
    //Serial.print("\r\n----------SALIR MODO COMANDO----------*\n");
  do{
    // Serial.println("---");
    //Ejecucion comando de salida
     Serial1.println("---");
     //Leer respuesta de dispositivo
     leer_data();
     //Escribir respuesta de dispositivo
    // escribir_data();
     //Si dispositivo responde con END se confirma salida
     //de lo contrario se realiza nuevamente la accion
  }while(data.compareTo("END")!=0);
//    Serial.print("\r\n----------------FIN-------------------*\n");

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
    delay(100);
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
      //Ingresar a modo comando
       modo_comando();
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
         // byte in = 'IQ';
        // Serial.write(in); // local echo
          Serial1.println("IQ");
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
    //Salir Modo Comando
    salir_modo_comando();
    buscar(direcciones);
    mac = cercano();
    Serial.print("MAC cercana: ");Serial.println(mac);
    int a = editar_conexion(mac);
}
int Bluetooth::editar_conexion(String mac){
  modo_comando();
  Serial1.write("SR,");
  Serial1.write(mac.c_str());
  Serial1.write(13);
  Serial1.write(10);  
  delay(100);
  leer_data();
  escribir_data();
  Serial1.println("GR");
  delay(100);
  leer_data();
  escribir_data();
  salir_modo_comando();
}


//Retorna la direccion del dispositivo que se encuentre mas cerca
//Segun RSSI
String Bluetooth::cercano(){
  int aux=-1000;
  int mac_indice;
  for(int i=0;i<10;i++){
    if(encontradas[i]==1){
      if(aux<=rssi[i]){
        aux=rssi[i];
        mac_indice=i;
      }
    } 
  }
  
  for(int x=0;x<10;x++){
    encontradas[x]=0;
  }
  
  return direccion[mac_indice];
}

//Funcion para encontrar mac y RSSI
//por dispositivo encontrado max 10 direcciones
void Bluetooth::buscar(char *data){
  char * pch;
  int n=0;
  int cantidad_direcciones=9;
  int posicion_directorio=-1;
  //Serial.println(data);
  pch = strtok (data,"\n,");
  while (pch != NULL){
  // Serial.println(pch);
    while(cantidad_direcciones>-1){
      if(((String)pch).compareTo(direccion[cantidad_direcciones])==0){
        encontradas[cantidad_direcciones]=1;
         pch = strtok (NULL, ",\n");
         pch = strtok (NULL, ",\n");
         pch = strtok (NULL, ",\n");
         rssi[cantidad_direcciones]=atoi(pch);

       //Serial.println("ENCONTRADA");
      }
        cantidad_direcciones--;
      }
      cantidad_direcciones=9;
    
   pch = strtok (NULL, ",\n");
  }
  Serial.println("Direccion\tRSSI");
  for(int x=0;x<10;x++){
    if(encontradas[x]==1){
      Serial.print(direccion[x]);Serial.print("\t");Serial.println(rssi[x]);
    }
  }
  
  
}

void Bluetooth::menu(){
 //Explora Dispositivos en el rango del dispositivo 
 Serial.println("Explorando");
 digitalWrite(pin_explora,HIGH);
  explorar();
    //Pausa entre cada exploracion
  Serial.println("Pausa explorando");
  digitalWrite(pin_explora,LOW);
//Lee informacion que provenga desde el dispositivo
    leer_data();
    if(data.length()>2){
      escribir_data();
    }
    if(data.compareTo("salir")){
      mac_cercana="";
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
