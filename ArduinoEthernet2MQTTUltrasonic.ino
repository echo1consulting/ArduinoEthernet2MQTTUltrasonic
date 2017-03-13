/**
   Arduino Ethernet2 MQTT Example - Non-blocking / Reconnect
   This sketch demonstrates how to keep the client connected
   using a non-blocking reconnect function. If the client loses
   its connection, it attempts to reconnect every x seconds
   without blocking the main loop.
*/

#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

// Initialize the Ultrasonic Library
Ultrasonic ultrasonic(8, 9);

// Initialize the Ethernet Client
EthernetClient ethernetClient;

// Set the Ethernet MAC Address
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xA9, 0x8B };

// Create an instance of the MQTT Client
PubSubClient client;

// Set the MQTT Server
IPAddress server(192, 168, 1, 167);

// Initialize the last reconnect attempt
long lastReconnectAttempt = 0;

/**
   Define a callback for when a message arrives on a topic
   @param topic char
   @param payload byte
   @param length int
*/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
}

/**
   Define a method that handles connecting to the MQTT Broker
   @return bool
*/
boolean reconnect() {
  Serial.println("Initializing MQTT connection...");
  if (client.connect("arduinoClient")) {
    Serial.println("Connected to MQTT Broker...");
    client.publish("ProjectX/BuildingX/SensorX/UnitX", "Hello World");
    Serial.println("Successfully published to topic...");
    if (client.subscribe("ProjectY/BuildingY/SensorY/UnitY")) {
      Serial.println("Successfully subscribed to topic...");
    } else {
      Serial.println("Failed to subscribe to topic...");
    }
  } else {
    Serial.print("failed, Return Code: ");
    Serial.println(client.state());
  }
  return client.connected();
}

/**
   The main setup
*/
void setup() {

  // Bug fix for the Ethernet2
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // Set the baud rate
  Serial.begin(9600);

  // Initializing Ethernet
  Serial.println("Initializing Ethernet...");

  // Wait for IP address to be resolved
  while (Ethernet.begin(mac) != 1) {

    Serial.println("Error getting an IP address via DHCP, trying again...");

    delay(1000);

  }

  // Print the ethernet gateway
  Serial.print("Gateway IP: ");
  Serial.println(Ethernet.gatewayIP());

  // Print your local IP address:
  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());

  // Print Ethernet OK
  Serial.println("Ethernet Initialized...");

  // Set the ethernet client
  client.setClient(ethernetClient);

  // Set the mqtt server
  client.setServer(server, 1883);

  // Set the callback
  client.setCallback(callback);

  // Reset the last reconnect attempt
  lastReconnectAttempt = 0;

  // Set a delay
  delay(1500);

}

/**
   The main loop
*/
void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {

    char buffer[10];

    dtostrf(ultrasonic.distanceRead(), 0, 0, buffer);

    client.publish("ProjectX/BuildingX/UltrasonicX/CM", buffer);

    Serial.println(buffer);

    client.loop();

    delay(1000);

  }
}
