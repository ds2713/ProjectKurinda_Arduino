#include <WiFiNINA.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

const char* server = "YOUR_SERVER_IP"; // e.g. 192.168.1.100
int port = 5000;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  float value = random(20, 30); // replace with sensor reading

  if (client.connect(server, port)) {
    client.println("POST /data HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(20);
    client.println();
    client.print("{\"value\":");
    client.print(value);
    client.println("}");
  }

  client.stop();
  delay(10000);
}