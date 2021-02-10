#include <SoftwareSerial.h>

SoftwareSerial ss(8, 9);

String mensagens[] = {"[22.1,19.3,98.5,27.6,00.0,-22323456,-42345432,HH:mm:ss]", "[22.1,19.3,28.7,77.6,20.0,-22020101,-42109722,HH:mm:ss]", "[22.1,19.3,28.7,77.6,00.0,-22020101,-42109722,HH:mm:ss]"};

void setup() {
  ss.begin(9600);
  Serial.begin(9600);
}

int cont = 0;

void loop() {
  ss.println(mensagens[cont]);
  
  Serial.println("Arduino nano manda: ");
  Serial.println(mensagens[cont]);

  
  cont = cont+1;

  
  if(cont == 3){
    cont = 0;
  }
   delay(1000);
 
}
