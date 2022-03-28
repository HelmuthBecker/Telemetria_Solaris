#include "FS.h"                     //Biblioteca para criar e editar arquivos no SD
#include "SD.h"                     //Biblioteca para gerenciar o cartão SD
#include "SPI.h"                    //Biblioteca para comunicação SPI

char mensagem[68];                  //Vetor que armazena os dados recebidos pela serial
byte a = 0;                         //Contador de Char recebidos

#define RXD2 13                     //Define o pino Rx da Serial 2
#define TXD2 12                     //Define o pino Tx da Serial 2

//Função que inicia o cartão SD
void iniciarSD(){                                 
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
 }

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  //Serial.print("Read from file: ");
  while(file.available()){
   char c = file.read();
    
    if((a == 0 && c != '[')){
      a = 0;
      continue;
    }
     mensagem[a] = c;
      a++;
      
      delay(50);
      if ((a == 64 && mensagem[63] == ']')){                       //Encerra a recepção caso receba 58 char, evitando receber dado além do limite
        //Serial.println(mensagem[45]);
        Serial.println(mensagem);
        Serial2.print(mensagem);
        mensagem[a] = '\0';
        a = 0;

        continue;
       } else if ((a == 64 && mensagem[63] != ']')){
        Serial.println("ERRO");
        mensagem[a] = '\0';
        a = 0;

        continue;
       }
     }
 file.close();
}

void setup() {
  Serial.begin (115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println();

  iniciarSD();

}

void loop() {
  readFile(SD, "/logSol.txt");
 delay(1000);
 }
