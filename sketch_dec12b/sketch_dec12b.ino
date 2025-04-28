#include <WiFi.h>
#include "DHT.h"

// WiFi credentials
const char* ssid = "HARSH_RAJ 4605";  // Your WiFi SSID
const char* password = "98765432";  // Your WiFi password

WiFiServer server(80);

// Pin assignments
#define LED1_PIN 5
#define LED2_PIN 18
#define FAN1_PIN 2
#define FAN2_PIN 4
#define DHT_PIN 15
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// Status tracking
bool led1Status = false;
bool led2Status = false;
bool fan1Status = false;
bool fan2Status = false;

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(FAN1_PIN, LOW);
  digitalWrite(FAN2_PIN, LOW);

  // Initialize DHT sensor
  dht.begin();

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // Automatically control fans based on temperature
  float temperature = dht.readTemperature();
  if (!isnan(temperature)) {
    if (temperature > 21) {
      fan1Status = true;
      fan2Status = true;
      digitalWrite(FAN1_PIN, HIGH);
      digitalWrite(FAN2_PIN, HIGH);
    } else {
      fan1Status = false;
      fan2Status = false;
      digitalWrite(FAN1_PIN, LOW);
      digitalWrite(FAN2_PIN, LOW);
    }
  }

  // Handle client requests
  WiFiClient client = server.accept();
  if (client) {
    Serial.println("New Client Connected");

    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println(request);

        // Handle LED and Fan controls
        if (request.indexOf("/LED1=HIGH") != -1) {
          led1Status = true;
          digitalWrite(LED1_PIN, HIGH);
        } else if (request.indexOf("/LED1=LOW") != -1) {
          led1Status = false;
          digitalWrite(LED1_PIN, LOW);
        }
        if (request.indexOf("/LED2=HIGH") != -1) {
          led2Status = true;
          digitalWrite(LED2_PIN, HIGH);
        } else if (request.indexOf("/LED2=LOW") != -1) {
          led2Status = false;
          digitalWrite(LED2_PIN, LOW);
        }
        if (request.indexOf("/FAN1=HIGH") != -1) {
          fan1Status = true;
          digitalWrite(FAN1_PIN, HIGH);
        } else if (request.indexOf("/FAN1=LOW") != -1) {
          fan1Status = false;
          digitalWrite(FAN1_PIN, LOW);
        }
        if (request.indexOf("/FAN2=HIGH") != -1) {
          fan2Status = true;
          digitalWrite(FAN2_PIN, HIGH);
        } else if (request.indexOf("/FAN2=LOW") != -1) {
          fan2Status = false;
          digitalWrite(FAN2_PIN, LOW);
        }

        // Respond to the client
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();

        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<head><title>ESP32 Control</title></head>");
        client.println("<body>");
        client.println("<h1>Control LEDs and Fans</h1>");
        client.println("<p>Temperature: " + String(temperature) + " &deg;C</p>");
        client.println("<p>LED 1: " + String(led1Status ? "ON" : "OFF") + "</p>");
        client.println("<p>LED 2: " + String(led2Status ? "ON" : "OFF") + "</p>");
        client.println("<p>Fan 1: " + String(fan1Status ? "ON" : "OFF") + "</p>");
        client.println("<p>Fan 2: " + String(fan2Status ? "ON" : "OFF") + "</p>");

        // Control buttons
        client.println("<a href='/LED1=HIGH'>Turn LED 1 ON</a><br>");
        client.println("<a href='/LED1=LOW'>Turn LED 1 OFF</a><br>");
        client.println("<a href='/LED2=HIGH'>Turn LED 2 ON</a><br>");
        client.println("<a href='/LED2=LOW'>Turn LED 2 OFF</a><br>");
        client.println("<a href='/FAN1=HIGH'>Turn Fan 1 ON</a><br>");
        client.println("<a href='/FAN1=LOW'>Turn Fan 1 OFF</a><br>");
        client.println("<a href='/FAN2=HIGH'>Turn Fan 2 ON</a><br>");
        client.println("<a href='/FAN2=LOW'>Turn Fan 2 OFF</a><br>");

        client.println("</body>");
        client.println("</html>");
        break;
      }
    }

    client.stop();
    Serial.println("Client Disconnected");
  }
}
