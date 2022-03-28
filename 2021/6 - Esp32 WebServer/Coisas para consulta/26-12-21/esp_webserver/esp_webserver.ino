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

#include "esp_task_wdt.h"           //Biblioteca do watchdog (Reinicia o ESP em caso de travamentos, perda de WiFi ou fique 10seg sem receber dados pela serial)
#include "WiFi.h"                   //Biblioteca para gerenciar conexões wifi
#include "ESPAsyncWebServer.h"      //Biblioteca para criar o WebServer
#include "FS.h"                     //Biblioteca para criar e editar arquivos no SD
#include "SD.h"                     //Biblioteca para gerenciar o cartão SD
#include "SPI.h"                    //Biblioteca para comunicação SPI
 
File myFile;                        //Objeto File

#define RXD2 13                     //Define o pino Rx da Serial 2
#define TXD2 12                     //Define o pino Tx da Serial 2

//char mensagem[58];                //Vetor que armazena os dados recebidos pela serial
char mensagem[50];
byte a = 0;                         //Contador de Char recebidos

const char* SSID     = "SOLARIS_BOAT";            //SSID - nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "abcd1234";                //Senha da rede WI-FI que deseja se conectar

//Configurações de rede do wifi do esp32
IPAddress WifiIP        (192, 168, 0, 12);
IPAddress WifiGateway   (192, 168, 0, 1);
IPAddress WifiDNS       (192, 168, 0, 1);
IPAddress WifiSubnet    (255, 255, 255, 0);

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
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {                      //Se mantem em loop enquanto não efetuar conexão com a rede Wifi
    Serial.print('.');
    delay(1000);
  }
  //Serial.println(WiFi.localIP());
}   //Fim conectar_WiFi

void lerSerial(){
 
  while(Serial2.available()){
   char c = Serial2.read();
    
    if((a == 0 && c != '[')){
      a = 0;
      continue;
    }
     mensagem[a] = c;
      a++;
      delay(50);
      
      if ((a == 46 && mensagem[45] == ']')){                       //Encerra a recepção caso receba 58 char, evitando receber dado além do limite
        //Serial.println(mensagem[45]);
        Serial.println(mensagem);
        salvarSD(SD, "/log.txt", mensagem);
        enviarAPP();
        Serial2.end();                                      //Encerra a serial 2 para limpar o buffer
        delay(500);
        Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);      //Reinicia a serial 2
        mensagem[a] = '\0';
        a = 0;

        continue;
       } else if ((a == 46 && mensagem[45] != ']')){
        Serial.println("ERRO");
        mensagem[a] = '\0';
        a = 0;

        continue;
       }
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
}

//Evento disparado sempre que o Esp perder conexão
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Disconnected from WiFi access point");
  Serial.print("Perda de conexao Wifi. Código: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Tentando reconectar");
  WiFi.begin(SSID, PASSWORD);
}

//Função que responde as solicitações HTTP vindas do aplicativo instalado no tablet do painel
void enviarAPP (){                                                         
    server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", mensagem);
    });
}
 
void setup(){
  Serial.begin (115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println();

  iniciarSD();

 //Inicia os eventos
 WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
 WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
 WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

 esp_task_wdt_init(15, true);                //Habilita o watchdog configurando o timeout para 10 segundos
 esp_task_wdt_add(NULL);
  
 conectar_WiFi();
 server.begin();                              //Inicia o WebServer
}
 
void loop(){
 lerSerial();
 esp_task_wdt_reset();                        //Reinicia o WatchDog
 delay(500);
}
