#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "<ssid>";
const char* password = "<pwd>";

const char* mqttServer = "<mqtt-broker-ip>";
const int mqttPort = 1883;
const char* mqttTopic = "<mqtt_topic>";

const int ledPin = 5;

WiFiClient espClient;

PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;

  String topicStr = topic;
  payload[length] = 0;
  String recv_payload = String(( char *) payload);

  DeserializationError error = deserializeJson(doc, recv_payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  boolean device_state = doc["device_state"];

  Serial.println(device_state);

  if(device_state) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
   }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Łączenie z WiFi...");
  }
  Serial.println("Połączono z siecią WiFi");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Łączenie z serwerem MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Połączono z serwerem MQTT");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("Błąd połączenia z serwerem MQTT, kod błędu: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  client.loop();
}
