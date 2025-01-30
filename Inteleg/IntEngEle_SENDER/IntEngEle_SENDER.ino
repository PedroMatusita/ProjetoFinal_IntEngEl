//==================================================================================//

#include <CAN.h>
#include <math.h>
#define TX_GPIO_NUM   21  // Connects to CTX
#define RX_GPIO_NUM   22  // Connects to CRX
#define BASE_RPM 1200

int motorRPM = BASE_RPM;

int value;
int media;
int valordiox;

//==================================================================================//

void setup() {
  Serial.begin (115200);
  while (!Serial);
  delay (1000);

  Serial.println ("CAN Receiver/Receiver");

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

  pinMode(33, INPUT);
  pinMode(32, INPUT); 
}

//==================================================================================//

void loop() {

  value = analogRead(33);  // reads the analaog value from the CO sensor's AOUT pin
  valordiox = analogRead(32);

    Serial.print(value);
  Serial.println ("               ");

      Serial.print(valordiox);
  Serial.println ("               ");

  int mapCO = map(value, 300, 4000, 1200, 3000);
  int mapCO2 = map(valordiox, 3700, 10000, 1200, 3000);
  motorRPM = (mapCO + mapCO2) / 2;

        Serial.print(mapCO);
  Serial.println ("               ");

        Serial.print(mapCO2);
  Serial.println ("               ");

        Serial.print(motorRPM);
  Serial.println ("               ");

  Serial.print(motorRPM);
  canSender(motorRPM);
}

//==================================================================================//

void canSender(int motorRPM) {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print ("Sending packet ... ");

  int valor_lido = motorRPM;

  CAN.beginPacket (0x12);  //sets the ID and clears the transmit buffer
  // CAN.beginExtendedPacket(0xabcdef);
  for (int i = 4; i > 0; i--) {
    char digito = valor_lido / ((int) pow(10, i-1)) + '0';
    valor_lido = valor_lido % ((int) pow(10, i-1));
    CAN.write(digito);
  }
  CAN.endPacket();

  //RTR packet with a requested data length
  CAN.beginPacket (0x12, 3, true);
  CAN.endPacket();

  Serial.println ("done");

  delay (1000);
}


//==================================================================================//

// void loop() { canSender(); }

// //==================================================================================//

// void canSender() {
//   //==================================================================================//
//   //   limit = digitalRead(PIN_SENSOR_IN);  //reads the digital value from the
//   //   CO sensor's DOUT pin valorseggas = analogRead(35);

//   value = analogRead(33);  // reads the analaog value from the CO sensor's AOUT pin
//   valordioxgas = analogRead(32);
//   //==================================================================================//

//   // send packet: id is 11 bits, packet can contain up to 8 bytes of data
//   Serial.print("Sending packet ... ");

//   // Serial.println("   ");
//   // Serial.println(valorseggas);
//   // prints the CO value
//   //  Serial.print("Limit: ");
//   //  Serial.print(limit);//prints the limit reached as either LOW or HIGH
//   //  (above or underneath)
//   //    delay(1000);
//   //    if (limit == HIGH) {
//   //      digitalWrite(13, HIGH);  //if limit has been reached, LED turns on as
//   //      status indicator
//   //    } else {
//   //      digitalWrite(13, LOW);  //if threshold not reached, LED remains off
//   //    }
//   CAN.beginPacket(0x12);  // sets the ID and clears the transmit buffer
//   // CAN.beginExtendedPacket(0xabcdef);
//   if (value < 37) {
//     CAN.write(0);
//   } else {
//     CAN.write("CO: ");
//     value = value - 30;
//     for (int i = 3; i > 0; i--) {
//       char digito = value % ((int)pow(10, i)) + '0';
//       value /= 10;
//       CAN.write(digito);
//     }
//     // envio do co pela CAN
//     delay(500);
//     CAN.write("CO2: ");
//     valordiox = analogRead(32);
//     media = ((valordiox + valordioxgas) / 2);
//     for (int i = 3; i > 0; i--) {
//       char digito = media % ((int)pow(10, i)) + '0';
//       media /= 10;
//       CAN.write(digito);
//     }
//     // envio do co2 pela CAN
//   }
//   CAN.endPacket();

//   // RTR packet with a requested data length
//   CAN.beginPacket(0x12, 3, true);
//   CAN.endPacket();

//   Serial.println("done");

//   delay(1000);
// }

