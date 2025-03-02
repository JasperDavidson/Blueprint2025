#include <ESP8266WiFi.h>
double mean = 0.0;
double stdev = 0.0;

const char* ssid = "MIT";
const char* password = "Gr8G+|D/?}";
const char* host = "10.31.180.89";

WiFiclient client;

void setup() {
pinMode(A0, INPUT);
WiFi.begin(ssid, password);
while(WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
}
Serial.println();
Serial.print("IP Address (AP): ");
Serial.println(WiFi.localIP());
stdev = 1;

Serial.println("There is a pothole at: 42.3604° N, 71.0873° W");
if (client.connect(host, 80)) {
  String url = "/update?value=";
  url += String(stdev);
  client.print(String("GET ") + url + "HTTP/.\r\n" + "Host: " + host + "\r\n" + "Connection: keep-alive\r\n\r\n");
  delay(10);
  Serial.println("Response: ");
  while(client.availale()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
}
}

void loop() {
  // Your logic here
}
