#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Define the WIFI ID and the server IP adress
const char* ssid = "argie";
const char* password = "11081998";
const char* host = "192.168.43.37";

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  //Connecting to the WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Trying to connect to wifi");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
  
    while (!client.connect(host, 80)) {
      Serial.println("Trying to connect to host");
      delay(100);
    }
    
  //Connecting to the server
    if (client.connect(host, 80)) {
      Serial.print("Connected!");
      client.print(String("GET /coba.php") + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close \r\n" + "\r\n");
    }    
  }

}

void loop() {
  String IncomingString;
  String StringArduino = "";
  String ini;
  int i = 0;
  boolean StringReady = false;


  HTTPClient http;
  http.begin("http://192.168.43.37/coba.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  //Reading the data sent from Arduino Mega
  while (Serial.available()) {
    StringArduino = Serial.readString();
    StringReady = true;
  }

  //Posting the data to the server
  if (StringReady) {
    String postData = StringArduino;
    int httpCode = http.POST(postData);
    String payload = http.getString();
  }
  delay(3000);   
}
