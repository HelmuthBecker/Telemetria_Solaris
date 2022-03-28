#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "PubSubClient.h"
 
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

IPAddress local_IP(192,168,1,60);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
 
byte    MQTTserver[]  = { 192, 168, 1, 104 };     // IP do servidor MQTT
String  topico_str    = "/app1/dados/equipe00";       //Cria o endereço do tópico MQTT onde serão publicados os dados Ex:. /app/dados/equipe07 ou /app/dados/equipe10
#define ID_MQTT         "esp32_mqtt"                       //ID do cliente para publicação no broker MQTT

WiFiClient      espClient;               // Cria o objeto espClient
PubSubClient    MQTT(espClient);         //Objeto PubSubClient

AsyncWebServer server(80);

void conectar_MQTT(){                                          //Função que conecta ao MQTT
  if (!MQTT.connected()){
    Serial.println("FALHA MQTT");
    while (!MQTT.connected()){                   
      MQTT.connect(ID_MQTT);
      delay(1000);                                              // Aguarda 1 seg antes de tentar conectar novamente
    }
  }
}  //Fim conectar_MQTT

void publicar (){
  char topico_char[((topico_str.length())+1)];                 //Vetor que irá armazenar o endereço do tópico em formato char
  topico_str.toCharArray(topico_char, sizeof(topico_char));    //Converte o endereço criado em char e atribui ao vetor topico_char

    MQTT.beginPublish(topico_char,3,false);                    //Inicia a publicação no MQTT      **Parâmetros - topico_char(endereço do topico MQTT) / qtdCharRcb(quantos caracteres serão publicados) / false(Não alterar)**                 
    MQTT.print("abc");                                         //Publica a mensagem
    MQTT.endPublish();                                         //Encerra a publicação no MQTT
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();

 // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  MQTT.setServer(MQTTserver, 1883);           //Inicia e define IP e porta do servidor MQTT
  conectar_MQTT();                            //Conecta ao broker MQTT
  server.begin();
}
 
void loop(){
  server.on("/info1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "11.1|22.2|33.3|44.4");
  });
  publicar();
  conectar_MQTT();
  delay(2000);
  
  }
