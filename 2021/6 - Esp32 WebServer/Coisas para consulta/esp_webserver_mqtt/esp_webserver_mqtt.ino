/*
 * O conecte o módulo SD nos seguintes pinos:
 *
 * SD Card | ESP32
 *    CS       G5
 *    MOSI     G23
 *    MISO     G19
 *    VCC      5V
 *    CLK      G18
 *    GND      GND
 */

#include "WiFi.h"                   //Biblioteca para gerenciar conexões wifi
#include "ESPAsyncWebServer.h"      //Biblioteca para criar o WebServer
#include "PubSubClient.h"           //Biblioteca MQTT
#include "FS.h"                     //Biblioteca para criar e editar arquivos no SD
#include "SD.h"                     //Biblioteca para gerenciar o cartão SD
#include "SPI.h"                    //Biblioteca para comunicação SPI
 
File myFile;                        //Objeto File

#define RXD2 13
#define TXD2 12

char mensagem[58];
byte a = 0;

const char* SSID     = "FOGUETE";        //SSID - nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "insert45";       //Senha da rede WI-FI que deseja se conectar

byte    mac[6];                                     //Variavel que irá armazenar o MAC do wifi do ESP
byte    MQTTserver[]  = { 192, 168, 1, 104 };       //IP do servidor MQTT
String  topico_str    = "/app1/dados/equipe00";     //Cria o endereço do tópico MQTT onde serão publicados os dados Ex:. /app/dados/equipe07 ou /app/dados/equipe10
#define ID_MQTT         "esp32"                     //ID do cliente para publicação no broker MQTT

WiFiClient      espClient;               //Cria o objeto espClient
PubSubClient    MQTT(espClient);         //Objeto PubSubClient

AsyncWebServer server(80);               //Porta de comunicação do WebServer

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

void conectar_MQTT(){                                           //Função que conecta ao MQTT
  if (!MQTT.connected()){
    //Serial.println("FALHA MQTT");
    MQTT.connect(ID_MQTT);
  }
}  //Fim conectar_MQTT

void publicar (){                                              //Função que publica mensagem via MQTT
  char topico_char[((topico_str.length())+1)];                 //Vetor que irá armazenar o endereço do tópico em formato char
  topico_str.toCharArray(topico_char, sizeof(topico_char));    //Converte o endereço DE STRING para CHAR e atribui ao vetor topico_char

    MQTT.beginPublish(topico_char,58,false);                   //Inicia a publicação no MQTT      **Parâmetros - topico_char(endereço do topico MQTT) / qtdCharRcb(quantos caracteres serão publicados) / false(Não alterar)**                 
    MQTT.println(mensagem);                                    //Publica a mensagem
    MQTT.endPublish();                                         //Encerra a publicação no MQTT
}  //Fim publicar

void conectar_WiFi() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}   //Fim conectar_WiFi

void enviarAPP (){                                                         //Função que responde as requisições HTTP
    server.on("/info1", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", mensagem);
    });
}
 
void setup(){
  Serial.begin (115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println();

  iniciarSD();

  conectar_WiFi();
  MQTT.setServer(MQTTserver, 1883);           //Inicia e define IP e porta do servidor MQTT
  conectar_MQTT();                            //Conecta ao broker MQTT
  server.begin();                             //Inicia o WebServer
}
 
void loop(){
  conectar_MQTT();                           //Verifica o status da conexão do MQTT

   //Recebe dados pela Serial 2
    while (Serial2.available() > 0){
      mensagem[a] = Serial2.read();
      a++;
    }

  //Caso a quantidade de char recebidos seja 58 (mensagem completa), publica no MQTT, Salva no cartão SD, envia para o APP do painel
  if (a == 58){
      a = 0;
      Serial.println(mensagem);
      salvarSD(SD, "/log.txt", mensagem);
      publicar();      
      enviarAPP();      
     }
     delay(500);
}
