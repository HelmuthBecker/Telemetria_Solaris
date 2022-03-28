/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    CS       G5
 *    MOSI     G23
 *    MISO     G19
 *    VCC      5V
 *    CLK      G18
 *    GND      GND
 */

#include "FS.h"
#include "SD.h"
#include "SPI.h"

File myFile;

char mensagem2[] = "ABCDE";

void iniciarSD(){
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
 }

void salvarSD(fs::FS &fs, const char * path, const char * message){       //Função que salva os dados recebidos no cartão SD
  myFile = fs.open(path, FILE_APPEND);                                    //Abre o arquivo para edição
  if(myFile.println(message)){
    myFile.close();                                  //Fecha o arquivo após a edição
    Serial.println("Salvo SD");                      //Exibe mensagem de confirmação de edição concluida
    return;
  }else {
    Serial.println("ERRO SALVAR SD");
    return;
   }
} //Fim salva_SD

void setup(){
    Serial.begin(115200);
    iniciarSD();    
}

void loop(){
  salvarSD(SD, "/hello.txt", mensagem2);
  delay(3000);
}
