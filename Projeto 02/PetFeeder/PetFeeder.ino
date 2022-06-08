/*
 * Curso - Controle dispositivos remotamente com ESP8266
 * Por: Fábio Souza
 * 
 * Exemplo - MQTT - Adafruit IO
 * Assina tópico no servidor MQTT
 */

/************************** Configuração***********************************/
#include "config.h"
#include <Servo.h>


/************************ Mapeamento de IO *******************************/
#define L1 2   //pino de saida para acionamento da Lampada L1
static const int servoPin = 13;

Servo servo1;

/************************ Configuração dos tópicos *******************************/

// configura o tópico "fabiosouza_io/feeds/L1"
AdafruitIO_Feed *feedL1 = io.feed("L1");
// configura o tópico "fabiosouza_io/feeds/Alimentador"
AdafruitIO_Feed *feedServo = io.feed("Alimentador");
// configura o tópico "fabiosouza_io/feeds/FoodReset"
AdafruitIO_Feed *feedFoodReset = io.feed("FoodReset");
// configura o tópico "fabiosouza_io/feeds/Food"
AdafruitIO_Feed *feedFood = io.feed("Food");

int resetFood = 100;
int full = 100;
int porcao = 15;

/************************ Função setup *******************************/

void setup() {

  //configura pino da Lampada como saída
  pinMode(L1,OUTPUT); 
      servo1.attach(
        servoPin, 
        Servo::CHANNEL_NOT_ATTACHED, 
        45,
        120
    );

  // configura comunicação serial
  Serial.begin(115200);

  // Aguarda serial monitor
  while(! Serial);

  conectaBroker(); //função para conectar ao broker


}

/************************ Função loop *******************************/

void loop() {

  // processa as mensagens e mantêm a conexão ativa
  byte state = io.run();

  //verifica se está conectado
  if(state == AIO_NET_DISCONNECTED | state == AIO_DISCONNECTED){
    conectaBroker(); //função para conectar ao broker
  }
    
}

/****** Função de tratamento dos dados recebidos em L1***************/

void handleL1(AdafruitIO_Data *data) {

  // Mensagem 
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if(data->isTrue())
    digitalWrite(L1, HIGH);
  else
    digitalWrite(L1, LOW);
}

void handleServo(AdafruitIO_Data *data) {

  // Mensagem 
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if(data->isTrue()){
      servo1.write(0);
      delay(2000);
      servo1.write(180);
      delay(2000);
      feedFood->save(full-porcao);
      full-=porcao;
  }
}

void handleFoodReset(AdafruitIO_Data *data) {

  // Mensagem 
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if(data->isTrue()){
      feedFood->save(resetFood);
      full = 100;
  }
}

/****** Função para conectar ao WIFI e Broker***************/

void conectaBroker(){
  
  //mensagem inicial
  Serial.print("Conectando ao Adafruit IO");

  // chama função de conexão io.adafruit.com
  io.connect();

   // instancia um novo handler para recepção da mensagem do feed L1 
  feedL1->onMessage(handleL1);
  feedServo->onMessage(handleServo);
  feedFoodReset->onMessage(handleFoodReset);
  
  // Aguarda conexação ser estabelecida
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conectado
  Serial.println();
  Serial.println(io.statusText());

  // certifique-se de que todos os feeds obtenham seus valores atuais imediatamente
  feedL1->get();
  feedServo->get();
  feedFoodReset->get();
}
