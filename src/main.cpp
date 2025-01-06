/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Import required libraries
#include "WiFi.h"
#include "WifiManager.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// Set LED GPIO
const int ledPin = 22;
// Stores LED state
String ledState;

// Set stepper motor pins
const int stepPin1 = 12;
const int dirPin1 = 14;
const int stepPin2 = 26;
const int dirPin2 = 27;
const int  steps_per_rev = 42;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // wifi setup
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("Connected to wifi");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}
 
void loop(){
  // motor 1
  digitalWrite(dirPin1, HIGH);
  Serial.println("Spinning Clockwise...");


  for (int i = 0; i < steps_per_rev; i++)
  {
    digitalWrite(stepPin1, HIGH);
    delay(10);
    digitalWrite(stepPin1, LOW);
    delay(10);
  }
  delay(1000);

  digitalWrite(dirPin1, LOW);
  Serial.println("Spinning Anti-Clockwise...");

  for (int i = 0; i < steps_per_rev; i++)
  {
    digitalWrite(stepPin1, HIGH);
    delay(10);
    digitalWrite(stepPin1, LOW);
    delay(10);
  }
  delay(1000);


  // motor 2
  digitalWrite(dirPin2, HIGH);
  Serial.println("Spinning Clockwise 2...");


  for (int i = 0; i < steps_per_rev; i++)
  {
    digitalWrite(stepPin2, HIGH);
    delay(10);
    digitalWrite(stepPin2, LOW);
    delay(10);
  }
  delay(1000);

  digitalWrite(dirPin2, LOW);
  Serial.println("Spinning Anti-Clockwise 2...");

  for (int i = 0; i < steps_per_rev; i++)
  {
    digitalWrite(stepPin2, HIGH);
    delay(10);
    digitalWrite(stepPin2, LOW);
    delay(10);
  }
  delay(1000);  
}