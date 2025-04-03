#include <Arduino.h>
#include "WiFiConnection.h"
#include "Credentials.h"
#include "Servo_Ctrl.h"


WiFiConnection wifi(WIFI_SSID, WIFI_PASSWORD);
ServoControl miServo(23);
// Variable definition
// Ultrasonic
#define TRIG_PIN 5
#define ECHO_PIN 18
// Servo
#define SERVO_PIN 23
// Infrared
#define IR_PIN 19

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  Serial.begin(115200);
  wifi.begin();
}

void loop() {
  wifi.update();
  
  if (wifi.connected()) {
    // Lee el estado del sensor infrarrojo
    if (digitalRead(IR_PIN) == LOW) {
      Serial.println("Object detected!");
      
      // Reactiva el servo antes de moverlo nuevamente
      miServo.resume_Servo();
      
      miServo.open_Servo(15);  // Abre el servo
      delay(1000);             // Espera 1 segundo
      
      miServo.close_Servo(15); // Cierra el servo
      miServo.stop_Servo();    // Detiene la señal PWM
      Serial.println("Alimento dispensado.");
      delay(1000);
    } else {
      Serial.println("No object detected.");
    }
    
    

    // Envía un pulso de 10 microsegundos al pin TRIG
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Lee el tiempo que tarda el pulso en regresar al pin ECHO
    long duration = pulseIn(ECHO_PIN, HIGH);

    // Calcula la distancia en centímetros
    float distance = (duration * 0.034) / 2;

    // Muestra la distancia en el monitor serie
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Espera 500 ms antes de la siguiente medición
    delay(500);
  }
}