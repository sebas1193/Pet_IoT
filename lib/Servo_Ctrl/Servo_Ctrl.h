//servo_Ctrl.h
#ifndef SERVO_CTRL_H
#define SERVO_CTRL_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoControl {
  private:
    int servoPin;
    Servo servo1;
    int neutralAngle = 90;  // Ángulo neutro (punto de referencia)
    int currentAngle = 90;  // Comenzamos en el ángulo neutro

  public:
    // Constructor: inicializa el servo en el pin y lo posiciona en el ángulo neutro
    ServoControl(int pin) {
      servoPin = pin;
      servo1.attach(servoPin);
      servo1.write(neutralAngle);
      currentAngle = neutralAngle;
    }

    // Método para abrir el servo: mueve desde el ángulo neutro hacia la apertura
    // relativeAngle > 0: cantidad de grados para abrir (por encima del neutro)
    void open_Servo(int relativeAngle) {
      int targetAngle = neutralAngle + relativeAngle;
      targetAngle = constrain(targetAngle, 0, 180); // Limitar a 0-180 grados
      // Mover desde la posición actual hasta el objetivo
      if (currentAngle <= targetAngle) {
        for (int pos = currentAngle; pos <= targetAngle; pos++) {
          servo1.write(pos);
          delay(20);
        }
      } else {
        for (int pos = currentAngle; pos >= targetAngle; pos--) {
          servo1.write(pos);
          delay(20);
        }
      }
      currentAngle = targetAngle;
    }

    // Método para cerrar el servo: mueve desde el ángulo neutro hacia el cerrado
    // relativeAngle > 0: cantidad de grados para cerrar (por debajo del neutro)
    void close_Servo(int relativeAngle) {
      int targetAngle = neutralAngle - relativeAngle;
      targetAngle = constrain(targetAngle, 0, 180); // Limitar a 0-180 grados
      if (currentAngle >= targetAngle) {
        for (int pos = currentAngle; pos >= targetAngle; pos--) {
          servo1.write(pos);
          delay(20);
        }
      } else {
        for (int pos = currentAngle; pos <= targetAngle; pos++) {
          servo1.write(pos);
          delay(20);
        }
      }
      currentAngle = targetAngle;
    }

    // Método para detener el servo
    void stop_Servo() {
      servo1.detach();  // Detiene la señal PWM al servo
    }

    // (Opcional) Método para reactivar el servo cuando necesites moverlo nuevamente
    void resume_Servo() {
      servo1.attach(servoPin);
      servo1.write(currentAngle); // Asegura que el servo retoma en la posición actual
    }
};

#endif
