#include <SoftwareSerial.h>
#include "NewPing.h"
SoftwareSerial esp8266(11,12);
#define serialCommunicationSpeed 115200
#define DEBUG true

// Ultrasonic values
#define TRIGGER_PIN 10
#define ECHO_PIN 9
#define MAX_DISTANCE 100

// Other pins
#define LED 6

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
const int pothole_threshold = 10;
int pothole_depth;
int level_depth;

void setup() {

 Serial.begin(115200);
 esp8266.begin(115200);
 InitWifiModule();

 level_depth = sonar.ping_cm();
 pothole_depth = 0;
 int numPotholes = 0; 
 pinMode(LED, OUTPUT);
 Serial.println(level_depth);
}

void loop() {
  int sonar_value = sonar.ping_cm();

  if (sonar_value > pothole_threshold) {
    pothole_depth = sonar_value;
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
  }

  if(esp8266.available()){
    String request = esp8266.readStringUntil('\r');
    if (request.indexOf("POST") != -1) {
      // Extract data from the request
      String data = request.substring(request.indexOf("\r\n\r\n") + 4);
      Serial.println(data);
    } else if(esp8266.find("+IPD,") && pothole_depth != 0) {
      delay(1000);
      int connectionId = esp8266.read()-48;
      String webpage = "Pothole detected! Depth: " + String(pothole_depth);
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";

      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);

      String closeCommand = "AT+CIPCLOSE=";
      closeCommand+= connectionId;
      closeCommand += "\r\n";
      sendData(closeCommand, 3000, DEBUG);
      pothole_depth = 0;
      numPotholes++;
    }
  }
}

String sendData(String command, const int timeout, boolean debug){
  String response = "";
  esp8266.print(command); 
  long int time = millis();
  while((time +timeout) > millis ()){
    while(esp8266.available()){
      char c = esp8266.read();
      response+=c;
    }
  }

  if(debug) {
    Serial.print(response); 
  } 
  return response;
}

void InitWifiModule(){
  sendData("AT+RST\r\n", 2000, DEBUG);
  sendData("AT+CWJAP="MIT","Gr8G+|D/?}"\r\n", 2000, DEBUG); 
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG); 
  delay(1500);
  sendData("AT+CIFSR\r\n", 1500, DEBUG);
  delay(1500);
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG);
  delay(1500);
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG); 
}
