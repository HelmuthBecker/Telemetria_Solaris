/*
|
|  Sistema de aquisição de dados da embarcação Solaris
|  Autor: Luciano Resende Dias
|  Data: 20/12/2021
|
|
*/


//#include <Arduino.h> //Esta biblioteca é gerada automaticamente pelo PlatformIO quando selecionamos desenvolvimento de código para Arduino
#include <SoftwareSerial.h>
#include <TinyGPS.h>


// ------ Constantes do Projeto ------
#define sCorrente                   A0
#define calibracaoSensorCorrente    A1
#define sBat24                      A2
#define sBat12                      A3
#define sPlacas                     A4
#define mvPorAmpere                6.25            //fator de conversão do sensor de corrente 625mV/A
#define mvRef                     5000             //Tensão de referência do conversor A/D
#define fatorBat24                  10             //Fator do divisor de tensão da rede resistiva 
#define fatorBat12                  10
#define fatorPlacas                 10


// ------ Variáveis Globais e Objetos ------
TinyGPS encoder;
SoftwareSerial GPS(9, 10);   // RX, TX
SoftwareSerial ESP (2, 3);  // RX, TX

struct dados{
  long latitude, longitude;
  unsigned long velocidade, data, tempo, idade, iMotor, vPlacas, vBat12, vBat24;
  String textoEnviar;
};

struct dados medicao;
volatile byte tempoAtualizacao = 0;

// ------ Protótipos de Função ------
void GPS_encoder(void);
void imprimeValores(long, uint8_t, uint8_t);
void calculaValores(void);
void geraEnvio(void);
void configTIMER1(void);
void tempo(void);

void setup() {
  configTIMER1();
  GPS.begin(9600) ;
  ESP.begin(9600);
  Serial.begin(9600);
}

void loop() {
  GPS_encoder();
}

void GPS_encoder() {
  unsigned long tempoLeitura = millis();

  //Comando da bibliota SoftwareSerial que "escuta" o recebimento de dados em uma serial virtual por vez
  GPS.listen();
  boolean recebido = false;

  //rotina de tempo para que o código possa executar outras funções sem ficar preso à leitura do GPS
  while ((millis() - tempoLeitura) < 500) {
    while (GPS.available()) {
      char cIn = GPS.read();
      recebido = encoder.encode(cIn);
    }

    //Verifica se os dados recebidos pelo GPS são válidos
    if (recebido) {
      medicao.velocidade = encoder.speed();
      encoder.get_position(&medicao.latitude, &medicao.longitude);
      encoder.get_datetime(&medicao.data, &medicao.tempo, &medicao.idade);
//    encoder.crack_datetime(&medicao.ano, &medicao.mes, &medicao.dia, &medicao.hora, &medicao.minuto, &medicao.segundo, &medicao.milesimos, &medicao.idade);
      break;
    }
  }
}

void calculaValores(){
  //calcula valor da corrente do motor
 uint16_t ref = ((analogRead(calibracaoSensorCorrente) / 1023.0) * mvRef) * 2;
 int32_t amp = ((((analogRead(sCorrente) - analogRead(calibracaoSensorCorrente)) / 1023.0) * ref) / 6.25) * 1000;
    
  if(amp < 0) amp *= (-1);
  medicao.iMotor = amp; 
  
  /* Serial.print("ref: "); Serial.print("  "); Serial.print(ref); Serial.print("  "); Serial.print(analogRead(sCorrente)); Serial.print("  "); Serial.print(analogRead(calibracaoSensorCorrente)); Serial.print("  amp: "); Serial.println(amp); */
   
  //calcula o valor de cada tensão 
  medicao.vBat24 = (((analogRead(sBat24)/1023.0)*mvRef) * fatorBat24); 
  medicao.vBat12 = (((analogRead(sBat12)/1023.0)*mvRef) * fatorBat12); 
  medicao.vPlacas = (((analogRead(sPlacas)/1023.0)*mvRef) * fatorPlacas); 

  //Debug da medição das tensões
  //Serial.print(analogRead(sBat24));
  //Serial.print("\t"); Serial.print(analogRead(sBat12));
  //Serial.print("\t"); Serial.println(analogRead(sPlacas));
}

void geraEnvio(){
  ESP.listen();
  
  medicao.textoEnviar = String("[") + (medicao.vBat24 / 10000) + ((medicao.vBat24 % 10000) / 1000) + String(".") + 
                        (((medicao.vBat24 % 10000) % 1000) / 100) + String(",") + (medicao.vBat12 / 10000) + 
                        ((medicao.vBat12 % 10000) / 1000) + String(".") + (((medicao.vBat12 % 10000) % 1000) / 100) 
                        + String(",") + (medicao.vPlacas / 10000) + ((medicao.vPlacas % 10000) / 1000) + String(".") + 
                        (((medicao.vPlacas % 10000) % 1000) / 100) + String(",") + (medicao.iMotor / 10000) + 
                        ((medicao.iMotor % 10000) / 1000) + String(".") + (((medicao.iMotor % 10000) % 1000) / 100) + 
                        String(",") + (medicao.velocidade / 100) + ((medicao.velocidade % 100) / 10) + String(".") + 
                        ((medicao.velocidade % 100) % 10) + String(",") + (medicao.latitude) + 
                        String(",") + (medicao.longitude) + String(",") + 

                        //separa hh
    (((medicao.tempo / 100) - 30000) / 100000) + ((((medicao.tempo / 100) - 30000) % 100000) / 10000) + String(":") +

    //separa mm
    (((((medicao.tempo / 100) - 30000) % 100000) % 10000) / 1000) + ((((((medicao.tempo / 100) - 30000) % 100000) % 10000) % 1000) / 100) + String(":") +

    //separa ss
    (((((((medicao.tempo / 100) - 30000) % 100000) % 10000) % 1000) % 100) / 10) + (((((((medicao.tempo / 100) - 30000) % 100000) % 10000) % 1000) % 100) % 10) + String(",") + 
    //Separa dia
    (medicao.data / 100000) + ((medicao.data % 100000) / 10000) + String("/") + 
    //separa mes
    (((medicao.data % 100000) % 10000) / 1000) + ((((medicao.data % 100000) % 10000) % 1000) / 100) + String("/") + 
    //separa ano
    (((((medicao.data % 100000) % 10000) % 1000) % 100) / 10) + (((((medicao.data % 100000) % 10000) % 1000) % 100) % 10) +  
    String("]");
    ESP.println(medicao.textoEnviar);
    //Serial.println(medicao.textoEnviar);

    Serial.end();
    ESP.end();
    GPS.end();
    delay(500);
    Serial.begin(9600);
    ESP.begin(9600);
    GPS.begin(9600);
}

void configTIMER1(){
  cli();                      //desabilita todas as interrupções
  TCCR1A = 0x00;              //limpa o conteúdo do registrador
  TCCR1B = 0x00;              //limpa o conteúdo do registrador
  OCR1A = 19999;              //base de tempo de 10ms
  TCCR1B = 0x0A;              //modo compare and clear (CTC) com prescaler de 8
  TIMSK1 = (1<<OCIE1A);       //habilita a interrupção por comparação do timer
  sei();                      //habilita interrupções globais
}

ISR(TIMER1_COMPA_vect){
  tempoAtualizacao++;
  if(tempoAtualizacao == 100){
    
    calculaValores();
    geraEnvio();
    // Serial.println((((((medicao.data % 100000) % 10000) % 1000) % 100) % 10));
    tempoAtualizacao = 0;
  }
}

  
