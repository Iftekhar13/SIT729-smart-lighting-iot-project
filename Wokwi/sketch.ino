#include <WiFi.h>
#include <PubSubClient.h>

const int pirPin = 13;
const int lightPin = 34;
const int ledPin = 27;

const int darkThreshold = 700;

// Wokwi Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Public MQTT broker
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    String clientId = "smart-lighting-esp32-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pirPin, INPUT);
  pinMode(lightPin, INPUT);
  pinMode(ledPin, OUTPUT);

  connectWiFi();

  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  int motion = digitalRead(pirPin);
  int lightValue = analogRead(lightPin);

  bool isDark = lightValue > darkThreshold;

  if (motion == HIGH && isDark) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  String ledStatus = (motion == HIGH && isDark) ? "ON" : "OFF";

  // Publish values
  client.publish("smartlighting/motion",
                 motion == HIGH ? "Detected" : "No motion");

  client.publish("smartlighting/light",
                 String(lightValue).c_str());

  client.publish("smartlighting/led",
                 ledStatus.c_str());

  Serial.print("Motion: ");
  Serial.print(motion == HIGH ? "Detected" : "No motion");

  Serial.print(" | Light value: ");
  Serial.print(lightValue);

  Serial.print(" | LED: ");
  Serial.println(ledStatus);

  Serial.println("MQTT data published");

  delay(2000);
}