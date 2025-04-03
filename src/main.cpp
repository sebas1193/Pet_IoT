#include <Arduino.h>
#include "WiFiConnection.h"
#include "Credentials.h"
#include "Servo_Ctrl.h"
#include <WiFi.h>
#include <time.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Variables definition
// Ultrasonic
#define TRIG_PIN 5
#define ECHO_PIN 18
// Servo
#define SERVO_PIN 23
// Infrared
#define IR_PIN 19
time_t lastServoActivation = 0;  // Última activación del servo en epoch
bool servoActive = false; // Auxiliary variable to track servo state

WiFiConnection wifi(WIFI_SSID, WIFI_PASSWORD);
ServoControl miServo(SERVO_PIN);
WiFiClient espClient;
PubSubClient client(espClient);

//horario
// Configuración de zona horaria y servidor NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -5 * 3600; // GMT-5 para Colombia
const int daylightOffset_sec = 0;    // Sin horario de verano

// MQTT server configuration
void reconnectMQTT() {
  // Intenta conectarte al servidor MQTT
  while (!client.connected()) {
    Serial.print("Conectando al servidor MQTT...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("Conectado.");
      client.subscribe(MQTT_TOPIC); // Suscríbete si es necesario
    } else {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos...");
      delay(5000);
    }
  }
}

// LEctura de sensores
int lecture_ir_sensor() {
  return digitalRead(IR_PIN) == LOW ? 1 : 0;
}

int lecture_ultra_sensor() {
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
  
  return distance;
}

void sendMessage() {
  JsonDocument doc;
  doc["presence"] = lecture_ir_sensor();
  doc["level_food"] = lecture_ultra_sensor();
  doc["servo_active"] = servoActive; // Use the auxiliary variable
  
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M", &timeinfo);
    doc["date"] = timeString;
  } else {
    doc["date"] = "N/A";
  }

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  if (client.publish(MQTT_TOPIC, jsonBuffer)) {
    Serial.println("Mensaje enviado al servidor MQTT.");
  } else {
    Serial.println("Error al enviar el mensaje.");
  }
}

void setup() {
  Serial.begin(115200);
  wifi.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando hora...");
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  wifi.update();
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Obtén la hora actual
  if (wifi.connected()) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Error obteniendo la hora");
      return;
    }
    time_t currentTime = mktime(&timeinfo); // Hora actual en epoch
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M", &timeinfo);

    // Lee el estado del sensor infrarrojo
    if (digitalRead(IR_PIN) == LOW) {
      Serial.println("Object detected!");
      Serial.println(timeString);

      // Verifica si han pasado más de 2 horas desde la última activación del servo
      if (difftime(currentTime, lastServoActivation) > 2 * 3600) {
        // Reactiva el servo antes de moverlo nuevamente
        miServo.resume_Servo();
        miServo.dispense_servo(1, 15, 15); // 1 segundo, abre 15°, cierra 15°
        servoActive = true;               // Actualiza el estado del servo
        miServo.stop_Servo();             // Detiene la señal PWM
        lastServoActivation = currentTime; // Actualiza la última activación
        Serial.println("Alimento dispensado.");
        sendMessage(); 
      } else {
        Serial.println("El servo no se activa porque fue usado hace menos de 2 horas.");
        sendMessage(); 
      }

      sendMessage(); // Envía el mensaje JSON
      servoActive = false; // Reinicia el estado del servo
      delay(1000);
    } else {
      Serial.println("No object detected.");
      Serial.println(timeString);
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

  sendMessage(); // Envía los datos al servidor MQTT cada 4 segundos
  delay(2000);   // Ajusta el intervalo de envío según sea necesario
}