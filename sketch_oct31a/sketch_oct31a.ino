#include <CAN.h>

#define RX_GPIO_NUM 22
#define TX_GPIO_NUM 21

// ID da mensagem CAN
#define MOTOR_RPM_ID 0x100

// Pino do botão
#define BUTTON_PIN 5

// RPM do carro ligado sem acelerar
#define BASE_RPM 1000

int motorRPM = BASE_RPM;
bool varia = false;

void setup() {
  Serial.begin (9600);
  while (!Serial);
  delay (1000);
  
  Serial.println ("CAN Receiver");

  // Set the pins
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);

  // start the CAN bus at 500 kbps
  if (!CAN.begin (500E3)) {
    Serial.println ("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println ("CAN Initialized");
  }

  //botao acelerador
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  // Leitura do estado do botão (HIGH quando pressionado)
  if (digitalRead(BUTTON_PIN) == HIGH) {
    varia = true;
  } else {
    varia = false;
  }

  if (varia) {
    // Acelera o motor
    motorRPM += 100;
  } else {
    // Desacelera o motor
    motorRPM -= 50;

    // Limita o RPM mínimo
    if (motorRPM < BASE_RPM) {
      motorRPM = BASE_RPM;
    }
  }

  // Criação da mensagem CAN
  CAN.beginPacket(MOTOR_RPM_ID);
  CAN.write(highByte(motorRPM));
  CAN.write(lowByte(motorRPM));
  CAN.endPacket();

  Serial.print("Mensagem CAN enviada - RPM: ");
  Serial.println(motorRPM);

  //RTR packet with a requested data length
  CAN.beginPacket (0x12, 3, true);
  CAN.endPacket();

  delay(1000);  
}