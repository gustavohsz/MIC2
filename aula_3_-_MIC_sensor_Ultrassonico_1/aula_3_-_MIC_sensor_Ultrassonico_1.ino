#include <HCSR04.h>

#define LED 6
int triggerPin = 13;
int echoPin = 12;
UltraSonicDistanceSensor distanciaSensor(triggerPin, echoPin);

void setup () {
    Serial.begin(9600);  
    pinMode(LED, OUTPUT);
}

void loop () {
    
    double distancia = distanciaSensor.measureDistanceCm();
    Serial.print(distancia);
    Serial.println(" cm");  

    digitalWrite(LED, !digitalRead(LED));
    int tempo = (int)distancia;
    delay(tempo + 30);
    
}
