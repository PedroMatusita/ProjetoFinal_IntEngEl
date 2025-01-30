#include <WiFi.h>
#include <PubSubClient.h>

//Atualize esses valores para de adaptar a sua rede WiFi
const char* ssid     = "Intengele_IOT";      //cuidado seja preciso aqui - use o hotspot
const char* password = "1Nt3n83!3";          //cuidado seja preciso aqui
const char* mqtt_server = "lprot.pea.usp.br";
#define mqtt_port         7883
#define MQTT_USER         ""
#define MQTT_PASSWORD     ""
#define MQTT_SERIAL_PUBLISH_CH   "/Intengele/Grupo_A_Tx"
#define MQTT_SERIAL_RECEIVER_CH  "/Intengele/Grupo_A_Rx" //pode ser o topico TX de outro grupo

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//cofigura uma conexao wifi - assegure-se de colocar SSID e password no inicio do programa
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.println("## ExemploMQTT started");
    Serial.println("## Build: " __DATE__ "-" __TIME__ );
    Serial.println("## File:  [" __FILE__ "]");
    Serial.print("## Connecting to ");
    Serial.print(ssid);
    Serial.print("-");
    Serial.print(password);
    Serial.print(" ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("## WiFi connected");
    Serial.print("## My IP address at local network: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  //Fica em loop enquanto nao se conecta
  while (!client.connected()) {
    //Cria um identificador para o cliente MQTT. Deve ser unico!
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    Serial.print("## Client Id = [");
    Serial.print(clientId.c_str()); Serial.println("]");

    //Tenta conectar ao broker
    Serial.println("## Attempting MQTT connectiont to broker");
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("## Client connected");
      
      //Se conectou faz uma publicacao de algo no topico desejado
      client.publish(MQTT_SERIAL_PUBLISH_CH, "Testando 1 2 3\n");
      
      //e subscreve ao topico desejado
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
      
      Serial.println("## Connected ok. Published and subscribed OK");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("## Trying again in 5 seconds");
      //Aguarda algum tempo antes de tentar novamente
      delay(5000);
    }
  }
}

//rotina que eh chamada a cada publicacao num topico subscrito
void callback(char* topic, byte *payload, unsigned int length) {
    Serial.print("<< new message from broker. Topic:[");
    Serial.print(topic);
    Serial.print("] data length:");
    Serial.print(length);
    Serial.println(". Data follows...");
    Serial.write(payload, length);
    Serial.println(" ");
}

//setup do kit ESP32
void setup() {
  Serial.begin(115200);     //velocidade da serial
  Serial.setTimeout(500);   //timeout da porta para informacoes recebidas
  setup_wifi();
  Serial.print("## MQTT Broker at [");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.print(mqtt_port);
  Serial.println("]");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

//rotina principal
int    len = 0;     //tamanho de uma mensagem recebida
long   counter = 0; //contador
long   eventcnt = 0;
void loop() {
   counter++;
   
   client.loop(); //aqui ocorre uma varredura das comunicacoes, checando se o broker enviou alguma informacao nova
   
   //checa se ha dados na serial. Se houver pega ate 500 bytes digitados pelo usuario 
   //e publica no topico para quem quiser escutar e estiver assinando
   if (Serial.available() > 0) {
     char bfr[501];
     memset(bfr,0, 501);
     len = Serial.readBytesUntil( '\n',bfr,500);
     if (!client.connected()) {
        Serial.println("## Broker not connected. Reconnecting");
        reconnect();
     }
     client.publish(MQTT_SERIAL_PUBLISH_CH, bfr);

     bfr[len]=0;
     Serial.print(">> Published to topic: [");
     Serial.print(MQTT_SERIAL_PUBLISH_CH);
     Serial.print("] Data: [");
     Serial.print(bfr);
     Serial.println("]");
   }
   
   if(counter>100000) //caso nao haja atividade de envio ou recebimento
   {
    if (!client.connected()) {
      Serial.println("## Broker not connected. Reconnecting");
      reconnect();
    }
    char bfr[501];
    //monta uma string com um contador, e envia esse contador no topico de publicacao
    snprintf(bfr, 501, "Cnt: %d", eventcnt++);
    client.publish(MQTT_SERIAL_PUBLISH_CH, bfr);
    counter=0;
   }
 }