/************************** Configuração***********************************/
#include "config.h"

#define BUZZER 15 //BUZZER PARA AVISOS SONOROS
#define SENSOR 34 //SENSOR LDR
#define VENTILACAO 4 // CONTROLE RELE DO VENTILADOR

float varLDR;
float ativarAlarme = 0;

/**************************   DHT 11    **********************************/
#include "DHT.h"

#define DHTPIN 27         //pino para leitura do DHT11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

/**************************   RFID    **********************************/
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 21 //pinos RFID
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);

/**************************   Servidor de horário NTP ****************************/

#include <NTPClient.h> /* https://github.com/arduino-libraries/NTPClient */

WiFiUDP udp;
NTPClient ntp(udp, "pool.ntp.org", -3 * 3600, 60000);/* Cria um objeto "NTP" com as configurações.utilizada no Brasil */
String hora;

/************************ Mapeamento de IO *******************************/
#define L1 26 //pino de saida para acionamento da Lampada L1
#define L2 25 //pino de saida para acionemento da Lampada L2
#define L3 33 //pino de saida para acionemento da Lampada L3
#define L4 32 //pino de saida para acionemento da Lampada L4

/***************************** Timer *****************************************/
unsigned long previousMillis1 = 0; //ira armazenar ultimo update
const long interval1 = 15000;  //intervalo em milisegundos

unsigned long previousMillis2 = 0;
const long interval2 = 100;

/************************ Configuração dos tópicos *******************************/

// configura o tópico "GustavoZanetti/feeds/L1"
AdafruitIO_Feed *feedL1 = io.feed("L1");

AdafruitIO_Feed *feedL2 = io.feed("L2");

AdafruitIO_Feed *feedL3 = io.feed("L3");

AdafruitIO_Feed *feedL4 = io.feed("L4");

AdafruitIO_Feed *feedTexto = io.feed("Texto");

AdafruitIO_Feed *feedSensorUmidade = io.feed("sensorUmidade");

AdafruitIO_Feed *feedSensorTemperatura = io.feed("sensorTemperatura");

AdafruitIO_Feed *feedAlarmes = io.feed("Alarmes");

AdafruitIO_Feed *feedAtivadorAlarme = io.feed("AtivadorAlarme");

/************************ Função setup *******************************/
void setup() {

  //configura pino das Lampadas como saídas
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(VENTILACAO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SENSOR, INPUT);

  // configura comunicação serial
  Serial.begin(115200);

  // Aguarda serial monitor
  while (! Serial);

  dht.begin(); //Inicializa sensor de temperatura e umidade

  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  conectaBroker(); //função para conectar ao broker

  ntp.begin();           // Inicia o protocolo para relógio
  ntp.forceUpdate();    // Atualização

}

/************************ Função loop *******************************/
void loop() {

  // processa as mensagens e mantêm a conexão ativa
  byte state = io.run();

  //verifica se está conectado
  if (state == AIO_NET_DISCONNECTED | state == AIO_DISCONNECTED) {
    conectaBroker(); //função para conectar ao broker
  }

  if (millis() - previousMillis1 >= interval1) { //timer para não bloquear outros processos
    previousMillis1 = millis();
    enviaDHT();
  }

  hora = ntp.getFormattedTime(); //atualiza a variavel tipo string da hora, conectado pela internet
  int horaSensor = hora.toInt(); //converte a string hora para um inteiro (apenas digitos da hora)
  if ((horaSensor) >= 20 || (horaSensor) <= 5 || ativarAlarme) { //se o horário estiver entre o intervalo o alarme será acionado automaticamente
    leituraSensorSeguranca();
  }

  if (millis() - previousMillis2 >= interval2) { //timer para não bloquear outros processos
    previousMillis2 = millis();
    leituraCartao();
  }
  delay(10);
}

/*FUNÇÃO PARA LEITURA DO SENSOR LDR DA SEGURANÇA*/

void leituraSensorSeguranca(void) {

  varLDR = analogRead(SENSOR); //lê o sensor LDR
  if ((varLDR) <= 1000) {
    int i = 0;
    while (i <= 30) {
      digitalWrite(BUZZER, !digitalRead(BUZZER));
      delay(100);
      i++;
    }
    feedAlarmes -> save("Alerta de invasão");
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

/* FUNÇÃO PARA LEITURA DO CARTÃO RFID */
void leituraCartao(void) {
  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();

  if (conteudo.substring(1) == "B0 70 6C A3") //UID 1 - Cartao
  {
    Serial.println("Ola!");
    Serial.println();
    digitalWrite(BUZZER, HIGH); delay(100); digitalWrite(BUZZER, LOW);
    feedTexto -> save("Gustavo");
  }
}

/* FUNÇÃO PARA LER E ENVIAR PARA SERVIDOR MQTT DADOS DO SENSOR DHT11 */
void enviaDHT(void) {

  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(temperatura) || isnan(umidade)) {

    Serial.println("Falha na leitura do DHT11");
  }
  else
  {
    Serial.print("Leitura DHT11 - umidade: ");
    Serial.print(umidade);
    Serial.print("  temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");
    if (temperatura > 15 || umidade > 85) {
      digitalWrite(VENTILACAO, HIGH);
    } else {
      digitalWrite(VENTILACAO, LOW);
    }
    feedSensorUmidade-> save(umidade);
    feedSensorTemperatura-> save(temperatura);
  }
}

void handleAtivadorAlarme(AdafruitIO_Data *data) {

  // Mensagem
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if (data->isTrue())
    ativarAlarme = 1;
  else
    ativarAlarme = 0;
}

/* Função de tratamento dos dados recebidos em L1 / L2 / L3 / L4 */
void handleL1(AdafruitIO_Data *data) {

  // Mensagem
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if (data->isTrue())
    digitalWrite(L1, HIGH);
  else
    digitalWrite(L1, LOW);
}

void handleL2(AdafruitIO_Data *data) {

  // Mensagem
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if (data->isTrue())
    digitalWrite(L2, HIGH);
  else
    digitalWrite(L2, LOW);
}

void handleL3(AdafruitIO_Data *data) {

  // Mensagem
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if (data->isTrue())
    digitalWrite(L3, HIGH);
  else
    digitalWrite(L3, LOW);
}

void handleL4(AdafruitIO_Data *data) {

  // Mensagem
  Serial.print("Recebido  <- ");
  Serial.print(data->feedName());
  Serial.print(" : ");
  Serial.println(data->value());

  //Aciona saída conforme dado recebido
  if (data->isTrue())
    digitalWrite(L4, HIGH);
  else
    digitalWrite(L4, LOW);
}


/****** Função para conectar ao WIFI e Broker MQTT ***************/
void conectaBroker() {

  //mensagem inicial
  Serial.print("Conectando ao Adafruit IO");

  // chama função de conexão io.adafruit.com
  io.connect();

  // instancia um novo handler para recepção da mensagem do feed L1 / L2 / L3 / L4
  feedL1->onMessage(handleL1);
  feedL2->onMessage(handleL2);
  feedL3->onMessage(handleL3);
  feedL4->onMessage(handleL4);
  feedAtivadorAlarme->onMessage(handleAtivadorAlarme);

  // Aguarda conexação ser estabelecida
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conectado
  Serial.println();
  Serial.println(io.statusText());

  // certifique-se de que todos os feeds obtenham seus valores atuais imediatamente
  feedL1->get();
  feedL2->get();
  feedL3->get();
  feedL4->get();
  feedAtivadorAlarme->get();
}
