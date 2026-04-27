#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <ArduinoLowPower.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// ==== WIFI SETTINGS ====
const char ssid[] = "Nano33_AP";
const char password[] = "password123";
const char data_topic[] = "landslides/data";
const char alert_topic[] = "landlides/alerts";

// ==== MQTT SETTINGS ====
const char* mqtt_server = "192.168.4.2"; // the IP address of the MQTT broker
const int mqtt_port = 1883;

// ==== SAMPLE TIMING CONGIGURATION ====

const int SAMPLE_COUNT = 6; // Number of samples collected before averaging
const int TX_COUNT_MAX = 6; // Transmit every 6 samples (1 min)
const int SLEEP_INTERVAL_MS = 1000; // 10s

float acc_buffer[SAMPLE_COUNT] = {0};
float temp_buffer[SAMPLE_COUNT] = {0};
float soil_buffer[SAMPLE_COUNT] = {0};

float acc_sum = 0;
float temp_sum = 0;
float soil_sum = 0;

int buffer_index = 0;
int samples_collected = 0;
int tx_counter = 0;

unsigned long last_sample_time = 0;
unsigned long last_publish_time = 0;

// ==== SENSOR CONFIGURATION ====

const int site_id = 1; // fixed per device

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
  Serial.println("Connecting to WiFi...");
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

void transmit_data(){
  
  Serial.println("Publishing data...");

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);

  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();
    
  unsigned long now = millis();

  float acc_avg = acc_sum / samples_collected;
  float temp_avg = temp_sum / samples_collected;
  float soil_avg = soil_sum / samples_collected;

  Serial.print("Avg acceleration: ");
  Serial.println(acc_avg);

  Serial.print("Avg temp: ");
  Serial.println(temp_avg);

  Serial.print("Avg SM: ");
  Serial.println(soil_avg);

  String payload = "{";
  payload += "\"sid\":" + String(site_id) + ",";
  payload += "\"ts\":" + String(now) + ",";
  payload += "\"ac_avg\":" + String(acc_avg) + ",";
  payload += "\"tmp_avg\":" + String(temp_avg) + ",";
  payload += "\"sm_avg\":" + String(soil_avg);
  payload += "}";

  client.publish(data_topic, payload.c_str());

  Serial.print("Published avg: ");
  Serial.println(payload);

  Serial.println("Turning off WiFi to save power...");

  WiFi.end(); // disconnect WiFi to save power

}

void data_processing_loop(){

  Serial.println("Starting data processing loop...");

  // Get new data from sensors
  float new_acc = get_acceleration();
  float new_temp = get_temperature();
  float new_soil = get_soil_moisture();

  // subtract old values (only once buffer is full)
  if (samples_collected == SAMPLE_COUNT) {
    acc_sum -= acc_buffer[buffer_index];
    temp_sum -= temp_buffer[buffer_index];
    soil_sum -= soil_buffer[buffer_index];
  }

  // store new values
  acc_buffer[buffer_index] = new_acc;
  temp_buffer[buffer_index] = new_temp;
  soil_buffer[buffer_index] = new_soil;

  // add to sums
  acc_sum += new_acc;
  temp_sum += new_temp;
  soil_sum += new_soil;

  Serial.print("New acceleration: ");
  Serial.println(new_acc);

  Serial.print("New temp: ");
  Serial.println(new_temp);

  Serial.print("New SM: ");
  Serial.println(new_soil);

  Serial.print("Sum acceleration: ");
  Serial.println(acc_sum);

  Serial.print("Sum temp: ");
  Serial.println(temp_sum);

  Serial.print("Sum SM: ");
  Serial.println(soil_sum);

  // move buffer index
  buffer_index = (buffer_index + 1) % SAMPLE_COUNT;

  if (samples_collected < SAMPLE_COUNT) {
    samples_collected++;
  }

  Serial.println("Sample collected");

  // ==== PUBLISH EVERY 60s ====
  if (tx_counter >= TX_COUNT_MAX) {
    tx_counter = 0;
    transmit_data();
  } else {
    tx_counter++;
    Serial.print("TX counter: ");
    Serial.println(tx_counter);
  }

  return;

}

void setup() {
  Serial.begin(9600);
  delay(2000);
}

void loop() {


  while(1) {
    data_processing_loop();

    // delay(SLEEP_INTERVAL_MS);
     LowPower.sleep(SLEEP_INTERVAL_MS);
  }

}
