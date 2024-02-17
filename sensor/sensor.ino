#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 0
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "<ssid>";
const char* password = "<pwd>";
const char* mqtt_server = "<mqtt_server_ip>";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishSensorData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  char convertedTemp[10];
  char convertedHumid[10];

  dtostrf(t, 2, 1, convertedTemp);
  dtostrf(h, 2, 1, convertedHumid);

  if (isnan(t) || isnan(h))
  {
    Serial.println("Fail to read sensor data");
  } else
  {
    Serial.println(t);
    client.publish("temperature_sensor", convertedTemp);
    client.publish("humidity_sensor", convertedHumid);
    delay(500);
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  publishSensorData();
}
