#include <WiFiNINA.h>
#include <PubSubClient.h>

// ==== WIFI SETTINGS ====
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

// ==== MQTT SETTINGS ====
const char* mqtt_server = "192.168.1.100"; // your PC IP
const int mqtt_port = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// ==== CONNECT TO WIFI ====
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
}

// ==== CONNECT TO MQTT ====
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    if (client.connect("nano33iot-client")) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  // ==== Generate dummy data ====
  int site_id = random(1, 4);
  float temperature = random(200, 300) / 10.0;
  float acceleration = random(0, 100) / 10.0;

  // ==== Build JSON payload ====
  String payload = "{";
  payload += "\"site_id\":" + String(site_id) + ",";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"acceleration\":" + String(acceleration);
  payload += "}";

  // ==== Publish ====
  client.publish("sensors/data", payload.c_str());

  Serial.print("Published: ");
  Serial.println(payload);

  delay(5000);
}