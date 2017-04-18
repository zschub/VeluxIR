/*
  This a simple example of the aREST Library for the ESP8266 WiFi chip.
  See the README file for more details.

  Written in 2015 by Marco Schwartz under a GPL license.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <VeluxIR.h>
#include <Bounce2.h>

const int BUTTON_CMD_UP = 12;
const int BUTTON_CMD_DOWN = 13;

// On when wifi has failed.
const int FAULT_LED = 0;

// On when wifi connection is solid.
const int WIFI_LED = 2;

// Used to transmit IR data. Handled by Velux instance.
const int IR_LED = 4;

// On when a command is being transmitted.
const int TRANSMIT_LED = 5;

VeluxIR velux;

// Create aREST instance
aREST rest = aREST();

// Instantiate Bounce object for manual command switches
Bounce debounceCommandUp = Bounce();
Bounce debounceCommandDown = Bounce();

// WiFi parameters
const char* ssid = "";
const char* password = "";

// The port to listen for incoming TCP connections
const int LISTEN_PORT = 80;

// Debounce interval in ms
const int DEBOUNCE_DURATION = 5;

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Declare functions to be exposed to the API
int sendCommand(String params);

// True when the wifi is not connected or some other fault has occured.
boolean faultState = true;

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Setup the first button with an internal pull-up :
  pinMode(BUTTON_CMD_UP, INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debounceCommandUp.attach(BUTTON_CMD_UP);
  debounceCommandUp.interval(DEBOUNCE_DURATION); // interval in ms

  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_CMD_DOWN, INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debounceCommandDown.attach(BUTTON_CMD_DOWN);
  debounceCommandDown.interval(DEBOUNCE_DURATION); // interval in ms

  velux.init(IR_LED);

  //Setup disgnostic LEDs :
  pinMode(WIFI_LED, OUTPUT);
  pinMode(FAULT_LED, OUTPUT);
  pinMode(TRANSMIT_LED, OUTPUT);

  digitalWrite(WIFI_LED, LOW);
  digitalWrite(FAULT_LED, HIGH);
  digitalWrite(TRANSMIT_LED, LOW);

  // Function to be exposed
  rest.function("led", sendCommand);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("1");
  rest.set_name("living");

  // Implement a simple blink while searching for wifi.
  boolean ledState = false;

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    ledState = !ledState;
    digitalWrite(WIFI_LED, ledState);
    digitalWrite(FAULT_LED, !ledState);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(WIFI_LED, LOW);
  digitalWrite(FAULT_LED, HIGH);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {

  // Update the Bounce instances :
  debounceCommandUp.update();
  debounceCommandDown.update();

  // Get the updated value :
//  int sendUp = debounceCommandUp.read();
//  int sendDown = debounceCommandDown.read();

  if (debounceCommandUp.fell()) {
    // Manual command for motor 1, UP
    sendCommand ("0");
  }
  else if (debounceCommandDown.fell()) {
    sendCommand ("1");
  }
  else {

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
}

// Custom function accessible by the API
int sendCommand(String params) {

  digitalWrite (TRANSMIT_LED, HIGH);
  
  int dirAsInt = params.toInt();
  VeluxIR::command dirAsEnum = (VeluxIR::command)dirAsInt;
  velux.transmit (0, dirAsEnum);

  Serial.print ("Params: ");
  Serial.print (params);
  Serial.print (" Command: ");
  Serial.println (dirAsEnum);

  digitalWrite (TRANSMIT_LED, LOW);

  return 1;
}
