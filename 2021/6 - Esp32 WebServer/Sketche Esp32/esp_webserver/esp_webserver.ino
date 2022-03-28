//este aqui

/*
 * Conecte o módulo SD nos seguintes pinos:
 *
 * SD Card | ESP32
 *    CS       G5
 *    MOSI     G23
 *    MISO     G19
 *    VCC      5V
 *    CLK      G18
 *    GND      GND
 */

#include "AsyncTCP.h"                     //Biblioteca que trabalha em conjunto com a biblioteca ESPAsyncWebServer.h
//#include "esp_task_wdt.h"               //Biblioteca do watchdog (Reinicia o ESP em caso de travamentos, perda de WiFi ou fique 10seg sem receber dados pela serial)
#include "WiFi.h"                         //Biblioteca para gerenciar conexões wifi
#include "ESPAsyncWebServer.h"            //Biblioteca para criar o WebServer
#include "FS.h"                           //Biblioteca para criar e editar arquivos no SD
#include "SD.h"                           //Biblioteca para gerenciar o cartão SD
#include "SPI.h"                          //Biblioteca para comunicação SPI
 
File myFile;                              //Objeto File

#define RXD2 13                           //Define o pino Rx da Serial 2
#define TXD2 12                           //Define o pino Tx da Serial 2

#define WIFI_LED  21                      //LED de status perda de sinal wifi

char mensagem[64];
byte a = 0;                               //Contador de Char recebidos
String temp;

unsigned long lastReceived = millis();    //Indica a ulltima vez que uma mensagem completa foi recebida

const char* SSID     = "SOLARIS_BOAT";    //SSID - nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "abcd1234";        //Senha da rede WI-FI que deseja se conectar

//Configurações de rede wifi do RASPBERRY para o esp32
IPAddress WifiIP        (192, 168, 0, 12);
IPAddress WifiGateway   (192, 168, 0, 1);
IPAddress WifiDNS       (192, 168, 0, 1);
IPAddress WifiSubnet    (255, 255, 255, 0);

//Configurações de rede wifi do celular para o esp32
//IPAddress WifiIP        (192, 168, 43, 160);
//IPAddress WifiGateway   (192, 168, 43, 1);
//IPAddress WifiDNS       (192, 168, 43, 1);
//IPAddress WifiSubnet    (255, 255, 255, 0);

//Rede de Casa
//const char* SSID     = "FOGUETE";            //SSID - nome da rede WI-FI que deseja se conectar
//const char* PASSWORD = "insert45";           //Senha da rede WI-FI que deseja se conectar


//Configurações de rede do wifi casa para o esp32
//IPAddress WifiIP        (192, 168, 1, 110);
//IPAddress WifiGateway   (192, 168, 1, 1);
//IPAddress WifiDNS       (192, 168, 1, 1);
//IPAddress WifiSubnet    (255, 255, 255, 0);


AsyncWebServer server(80);                          //Porta de comunicação do WebServer

//Função que inicia o cartão SD
void iniciarSD(){                                 
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
 }

//Função que salva o dados no cartão SD
void salvarSD(fs::FS &fs, const char * path, const char * message){       //Função que salva os dados recebidos no cartão SD
  myFile = fs.open(path, FILE_APPEND);                                    //Abre o arquivo para edição
  if(myFile.println(message)){
    myFile.close();                                                       //Fecha o arquivo após a edição
    Serial.println("Salvo SD");                                           //Exibe mensagem de confirmação de edição concluida
    return;
  }else {
    Serial.println("ERRO SALVAR SD");
    return;
   }
} //Fim salvarSD

//Função que conecta a rede WiFi
void conectar_WiFi() 
{
  WiFi.mode(WIFI_STA);                                         //Define o Ep32 como cliente
  WiFi.config(WifiIP, WifiDNS, WifiGateway, WifiSubnet);       //Define configurações fixas para IP, DNS, Gateay e Mascara de Subrede, antes de se conectar ao AP
  WiFi.begin(SSID, PASSWORD);                                  //Inicia a conexão na SSID e Password
  if (WiFi.status() != WL_CONNECTED) {                      //Se mantem em loop enquanto não efetuar conexão com a rede Wifi
     digitalWrite(WIFI_LED,  HIGH);      //Acende o led de status WiFi
  }
}   //Fim conectar_WiFi

void lerSerial(){
  if (Serial2.available() > 0) {
   while(Serial2.available() > 0){
   char c = Serial2.read();
    
    if((a == 0 && c != '[')){
      a = 0;
      continue;
    }
     mensagem[a] = c;
      a++;
      delay(50);
      
      if ((a == 64 && mensagem[63] == ']')){                      
        //Serial.println(mensagem);
        temp = mensagem;
        salvarSD(SD, "/log.txt", mensagem);
        mensagem[a] = '\0';
        a = 0;
        lastReceived = millis();
        Serial2.end();                                      //Encerra a serial 2 para limpar o buffer
        delay(500);
        Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);      //Reinicia a serial 2
        break;
       } else if ((a == 64 && mensagem[63] != ']')){
        //Serial.println(mensagem);
        //Serial.println("ERRO");
        mensagem[a] = '\0';
        a = 0;
        Serial2.end();                                      //Encerra a serial 2 para limpar o buffer
        delay(500);
        Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);      //Reinicia a serial 2
        break;
       } 
     }
  }
     if (millis() - lastReceived >= 15000){                                  //Caso fique mais de 20seg sem receber dados, esvazia a variável temp, para que o app exiba a mensagem de "Sem Sinal" na tela
            temp = "";
            //Serial.println("teste");
            //temp = "[00.0,00.0,00.0,00.0,00.0,-00000000,-00000000,00:00:00,00/00/00]";
            lastReceived = millis();
            Serial2.end();                                      //Encerra a serial 2 para limpar o buffer
            delay(500);
            Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);      //Reinicia a serial 2
     }
}

//Eventos de monitoramento do estado da conexão Wifi

//Evento disparado sempre que o Esp se conectar ao Wifi
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Conectado ao Wifi");
}

//Evento disparado sempre que o Esp adquirir IP
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("WiFi connected");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(WIFI_LED,  LOW);       //Apaga o led de status WiFi
}

//Evento disparado sempre que o Esp perder conexão
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Disconnected from WiFi access point");
  //Serial.print("Perda de conexao Wifi. Código: ");
  //Serial.println(info.disconnected.reason);
  //Serial.println("Tentando reconectar");
  digitalWrite(WIFI_LED,  HIGH);      //Acende o led de status WiFi
  WiFi.begin(SSID, PASSWORD);
}

void setup(){
  Serial.begin (115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  pinMode(WIFI_LED, OUTPUT);
  Serial.println();

  iniciarSD();

 //Inicia os eventos
 WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
 WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
 WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

 server.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
 request->send(200, "text/plain", temp);
  });

 //esp_task_wdt_init(15, true);                //Habilita o watchdog configurando o timeout para 10 segundos
 //esp_task_wdt_add(NULL);

 
 conectar_WiFi();
 server.begin();                              //Inicia o WebServer
}
 
void loop(){
 lerSerial();
 //esp_task_wdt_reset();                        //Reinicia o WatchDog
 delay(500);
}
