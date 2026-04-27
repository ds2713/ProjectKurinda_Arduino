#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "Nano33_AP";    // Name of your network
char pass[] = "password123"; // Must be 8+ characters

void setup() {
  Serial.begin(9600);
  
  // Start the access point
  WiFi.beginAP(ssid, pass);
  
  // Print network details
  Serial.print("AP SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("AP IP Address: ");
  Serial.println(ip);
}

void loop() {
  // Loop code
}