#include <WiFiNINA.h>
#include <PubSubClient.h>

// ==== WIFI SETTINGS ====
const char ssid[] = "Nano33_AP";
const char password[] = "password123";
const char data_topic[] = "landslides/data";
const char alert_topic[] = "landlides/alerts";

// ==== MQTT SETTINGS ====
const char* mqtt_server = "192.168.4.2"; // the IP address of the MQTT broker
const int mqtt_port = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Function to get acceleration data
float get_acceleration() {
  // Simulate acceleration data between -10 and 10 m/s^2
  float acc = (float) random(-10, 10);
  return acc;
};

// Function to get temperature data
float get_temperature() {
  // Simulate temperature data between 0.0 and 50.0 degrees Celsius
  float temp = (float) random(0, 50);
  return temp;
};

// Function to get soil moisture data
float get_soil_moisture() {
  // Simulate soil moisture data
  float moisture = (float) random(0, 100);
  return moisture;
};

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


  // ==== Collect sensor data ====
  int site_id = random(1, 4);
  float acceleration = get_acceleration();
  float temperature = get_temperature();
  float soil_moisture = get_soil_moisture();

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


