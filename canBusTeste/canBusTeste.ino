//==================================================================================//

#include <CAN.h>
#include <math.h>
// #include <Arduino.h>

#define TX_GPIO_NUM   21  // Connects to CTX
#define RX_GPIO_NUM   22  // Connects to CRX
#define PIN_SENSOR_CO 2   // sensor CO
#define LED_CALIBRAGEM 19 //  Led para calibrar sensor
#define LED_CALIBRAGEM_1 18 //  Led para calibrar sensor
#define MOTOR_RPM_ID 0x100
#define BASE_RPM 1000

int motorRPM = BASE_RPM;
char c;
int x = 1;

//==================================================================================//

void setup() {
  Serial.begin (115200);
  while (!Serial);
  delay (1000);

  Serial.println ("CAN Receiver/Receiver");

  // Set the pins
  pinMode(PIN_SENSOR_CO, INPUT);
  pinMode(LED_CALIBRAGEM, INPUT);
  pinMode(LED_CALIBRAGEM, OUTPUT);
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);

  // start the CAN bus at 500 kbps
  if (!CAN.begin (500E3)) {
    Serial.println ("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println ("CAN Initialized");
  }
}

//==================================================================================//

void loop() {
  canSender();
  //canReceiver();
}

//==================================================================================//

void canSender() {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print ("Sending packet ... ");

  // c = motorRPM % 10 + '0';

  // CAN.beginPacket (0x12);  //sets the ID and clears the transmit buffer
  // // CAN.beginExtendedPacket(0xabcdef);
  // CAN.write ('1'); //write data to buffer. data is not sent until endPacket() is called.
  // CAN.write ('2');
  // CAN.write ('3');
  // CAN.write ('4');
  // CAN.write ('5');
  // CAN.write ('6');
  // CAN.write ('7');
  // CAN.write (c);
  // CAN.endPacket();

  // CAN.beginPacket (0x12, 3, true);
  // CAN.endPacket();


  CAN.beginPacket (0x12);
  motorRPM = 100;

  for(int i = 3; i > 0; i--) {
    x = 1;
    for(int j = 0; j < (i-1); j++) {
      x = x*10;
    }
    c = motorRPM / x + '0';
    motorRPM = motorRPM % x;
    CAN.write(c);
  }

  CAN.endPacket();

  //RTR packet with a requested data length
  CAN.beginPacket (0x12, 3, true);
  CAN.endPacket();

  Serial.println ("done");

  delay (1000);
}

//==================================================================================//

// void canReceiver() {
//   // try to parse packet
//   int packetSize = CAN.parsePacket();

//   if (packetSize) {
//     // received a packet
//     Serial.print ("Received ");

//     if (CAN.packetExtended()) {
//       Serial.print ("extended ");
//     }

//     if (CAN.packetRtr()) {
//       // Remote transmission request, packet contains no data
//       Serial.print ("RTR ");
//     }

//     Serial.print ("packet with id 0x");
//     Serial.print (CAN.packetId(), HEX);

//     if (CAN.packetRtr()) {
//       Serial.print (" and requested length ");
//       Serial.println (CAN.packetDlc());
//     } else {
//       Serial.print (" and length ");
//       Serial.println (packetSize);

//       // only print packet data for non-RTR packets
//       while (CAN.available()) {
//         Serial.print ((char) CAN.read());
//       }
//       Serial.println();
//     }

//     Serial.println();
//   }
// }

// //==================================================================================//

