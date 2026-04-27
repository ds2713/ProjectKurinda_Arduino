#include <WiFiNINA.h>
#include <PubSubClient.h>

// ==== WIFI SETTINGS ====
const char ssid[] = "Nano33_AP";
const char password[] = "password123";
const char data_topic[] = "landslides/data";
const char alert_topic[] = "landlides/alerts";

// ==== MQTT SETTINGS ====
const char* mqtt_server = "192.168.4.2"; // your PC IP
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
    Serial.println("Connecting to MQTT...");

    if (client.connect("nano33iot-client")) {
      Serial.println("connected!");
    } else {
      Serial.println("failed, rc=");
      Serial.println(client.state());
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
  float acceleration = random(-10, 10);
  float temperature = random(200, 300) / 10.0;
  float soil_moisture = random(0, 100);

  // ==== Build JSON payload ====
  String payload = "{";
  payload += "\"sid\":" + String(site_id) + ",";
  payload += "\"ts\":" + String(millis()) + ",";
  payload += "\"ac\":" + String(acceleration) + ",";
  payload += "\"tmp\":" + String(temperature) + ",";
  payload += "\"sm\":" + String(soil_moisture);
  payload += "}";

  // ==== Publish ====
  client.publish(data_topic, payload.c_str());

  Serial.print("Published: ");
  Serial.println(payload);

  delay(5000);
}


