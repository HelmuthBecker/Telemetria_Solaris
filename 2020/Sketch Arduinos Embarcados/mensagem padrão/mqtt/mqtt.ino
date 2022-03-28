#include<SoftwareSerial.h>
#include<SD.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>

byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,0,222); //Endereço da shield
IPAddress server(192,168,0,252);  //Endereço do raspi

SoftwareSerial ss(8,9);
EthernetClient ethClient;
PubSubClient client(ethClient);

File myFile;
char mensagem[48];
byte index = 0;

//Da string
/**
 *  0 a 24 - medidas
 *  26 a 45 - localização
 *  [aa.a,bb.b,cc.c,dd.d,ee.e,-xx.xxxxxx,-yy.yyyyyy]
 */


 //Leitura do cartão SD
 //Pino 12
 //O led do arduino piscará numa frequência de 2 Hz, caso haja erro
void ler_cartao_SD(){
  bool t = false;
  while (1) {
    t = SD.begin(4);
    if(t){
      Serial.println("Cartão SD está pronto pra operar...");
      break;
    }
  }
}

//Reconectar ao servidor (Raspi)
//Pino 11
//O led do arduino piscará numa  frequência de 0.5 Hz, caso haja erros
void reconnect() {
  // Loop until we're reconnected
    if (client.connect("arduinoClient")) {
      //digitalWrite(6, LOW);
      Serial.println("Conexão realizada com sucesso...");
    }else{
        Serial.println("Falha na conexão...");
    }
}

//Publica os valores das grandezas coletadas
//das medidas
/*
 * [aa.a,bb.b,cc.c,dd.d,ee.e]
 */
void enviar_grandezas_medidas(){
  char medidas[27];

  medidas[0] = '[';

  for(byte a = 1; a < 25; a++){
    medidas[a] = mensagem[a];
  }
  medidas[25] =  ']';
  medidas[26] =  '\0';

    
  client.publish("app/medidas", medidas);
}

//Publica a localização coletada 
//Das coordenadas
/**
 * [-xxxxxxxx,-yyyyyyyy]
 */
 
void enviar_localizacao(){
  char loc[22];

  loc[0] = '[';

  for(byte a = 26; a < 47; a++){
    loc[a-25] = mensagem[a];   
  }
  loc[20] =  ']';
  loc[21] =  '\0';


  client.publish("app/loc", loc);
  
}

//Salva os dados coletados no cartão SD
void salvar_SD() {
  //abre o arquivo para edição


 // Serial.println("Salvando a mensagem...");
  myFile = SD.open("dados.txt", FILE_WRITE);

  //grava o que foi digitado no arquivo
  myFile.println(mensagem);

  //fecha e salva o arquivo após edição
  myFile.close(); 
}


void setup() { 
  
  Serial.begin(9600);
  ss.begin(9600);

  ler_cartao_SD();  //Tenta ler o cartão SD
  
  client.setServer(server, 1883);
  Ethernet.begin(mac, ip);
}

/**
 *  [aa.a,bb.b,cc.c,dd.d,ee.e,-xxxxxxxx,-yyyyyyyy] 
 */
void loop() {
  //Leitura da serial
  while(ss.available()){
    
    char c = ss.read(); 
    
    //mensagem inválida...
    if((index == 0 && c != '[')){
      index = 0;
      continue;
    }

    mensagem[index] = c;
    index = index+1;

    if(index == 47){
      mensagem[index] = '\0';
        
      salvar_SD();
      enviar_grandezas_medidas();
      enviar_localizacao();

      
      index = 0;

      continue;
    }    
  }
  
  //Tenta reconectar
   if (!client.connected()) {
    reconnect();
   }
  
  client.loop();
  delay(100);

}
