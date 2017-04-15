/*
  This a simple example of the aREST Library for the ESP8266 WiFi chip.
  See the README file for more details.

  Written in 2015 by Marco Schwartz under a GPL license.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <VeluxIR.h>

const int LED = 4;

VeluxIR velux;

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "parknet";
const char* password = "";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Declare functions to be exposed to the API
int sendCommand(String params);

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  velux.init(LED);

  // Function to be exposed
  rest.function("led", sendCommand);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("1");
  rest.set_name("living");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {

  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while (!client.available()) {
    delay(1);
  }
  rest.handle(client);

}

// Custom function accessible by the API
int sendCommand(String params) {

  int dirAsInt = params.toInt();
  VeluxIR::command dirAsEnum = (VeluxIR::command)dirAsInt;
  velux.transmit (0, dirAsEnum);

  Serial.print ("Params: ");
  Serial.print (params);
  Serial.print (" Command: ");
  Serial.println (dirAsEnum);

  return 1;
}
