//==================================================================================//

#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <CAN.h>
#include <vector>
#include <iostream>

using namespace std;

const char* ssid = "Intengele_IOT";
const char* password = "1Nt3n83!3";
const char* mqtt_server = "lprot.pea.usp.br";
#define mqtt_port         7883
#define MQTT_USER         ""
#define MQTT_PASSWORD     ""
#define MQTT_SERIAL_PUBLISH_CH   "/Intengele/Grupo_14_Tx"
#define MQTT_SERIAL_RECEIVER_CH  "/Intengele/Grupo_14_Rx"

#define TX_GPIO_NUM   21  // Connects to CTX
#define RX_GPIO_NUM   22  // Connects to CRX

unsigned long lastMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient);

//==================================================================================//

void setup() {
  delay(100);
  Serial.begin(115200);
  while (!Serial);
  delay (1000);

  Serial.println ("CAN Receiver/Receiver"); // Set the pins
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);
  if (!CAN.begin (500E3)) { // start the CAN bus at 500 kbps
    Serial.println ("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println ("CAN Initialized");
  }

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);

}

//==================================================================================//

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  StaticJsonDocument<80> doc;
  char output[80];

  int packetSize = CAN.parsePacket();


  long now = millis();


  if (packetSize) {
    // received a packet
    Serial.print ("Received ");

    if (CAN.packetExtended()) {
      Serial.print ("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print ("RTR ");
    }

    Serial.print ("packet with id 0x");
    Serial.print (CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print (" and requested length ");
      Serial.println (CAN.packetDlc());
    } else {
      Serial.print (" and length ");
      Serial.println (packetSize);

      int valor = 0;

      // only print packet data for non-RTR packets
      if (now - lastMsg > 5000) {
        for(int i = 4; CAN.available(); i--) {
          valor = valor + ((char) CAN.read() - '0')*((int) pow(10, i-1));
          doc["RPM"] = valor;
          // Serial.print ((char) CAN.read());
        }

        double CO2_emissao = interpolacao(valor,0);

        double CO_emissao = interpolacao(valor,1);

        doc["%CO2"] = CO2_emissao;
        doc["%CO"] = CO_emissao;

        serializeJson(doc, output);
        Serial.println(output);
        client.publish(MQTT_SERIAL_PUBLISH_CH, output);
      }
      Serial.println();
    }
    Serial.println();
  }
}

//==================================================================================//

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

//==================================================================================//

double interpolacao(int RPM, int opcao){
  vector<int> RPM_valores = {1200, 1500, 1800, 2100, 2400};

  vector<double> CO2_pc = {2.5, 3.0, 1.15, 1.25, 1.1};
  vector<double> CO_pc = {0.05, 0.06, 0.02, 0.03, 0.03};

  double interpolado;

  if (opcao == 0) //co2 
  {
    int i = 0;
    while (RPM > RPM_valores[i]) {
        i++;
    }

    interpolado = (((RPM - RPM_valores[i-1]) / static_cast<double>(RPM_valores[i] - RPM_valores[i-1])) * (CO2_pc[i] - CO2_pc[i-1])) + CO2_pc[i-1];

  }

  if (opcao == 1) //co
  {
      int i = 0;
    while (RPM > RPM_valores[i]) {
        i++;
    }

    interpolado = (((RPM - RPM_valores[i-1]) / static_cast<double>(RPM_valores[i] - RPM_valores[i-1])) * (CO_pc[i] - CO_pc[i-1])) + CO_pc[i-1];
  }

    return interpolado;
}
