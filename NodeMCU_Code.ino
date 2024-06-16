#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const char *ssid = "TP-Link_C502";
const char *password = "00970396";
const char *apiEndpoint = "http://192.168.0.100:8069/accessors/"; // Replace with your API endpoint

void setup() {
  Serial.begin(115200);
  // Serial1.begin(115200);
  connectToWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (Serial.available()) {
      String message = Serial.readString();
      if (message[0] == '*') {
        int endIndex = message.indexOf('\n');
        String otp = message.substring(1, endIndex);
        fetchDataFromAPI(otp);
      }
    }
    delay(2000); // Wait for 1 minute before fetching data again
  } else {
    connectToWiFi();
  }
}


void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void fetchDataFromAPI(String otp) {
  // Modify the API endpoint to include the received OTP
  String apiUrl = apiEndpoint + otp;

  WiFiClient client;
  HTTPClient http;
  http.begin(client, apiUrl);
  
  int httpResponseCode = http.GET();

  String payload = "";
  if (httpResponseCode == 200) {
    Serial.println("|Authenticated");
  } else {
    Serial.print("|Not authenticated");
    Serial.println(httpResponseCode);
  }

  http.end();
}