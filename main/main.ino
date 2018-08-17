// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Brzo.h"

#define DHTTYPE DHT11   // DHT 11

const char* ssid = "CenturyLink0110";
const char* password = "2j3evg2hgxsew6";



// Web Server on port 80
ESP8266WebServer server(80);

void handle_index() {
  float t = computeHeatIndex();
 
  char htmlIndex[400];
  snprintf ( htmlIndex, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Temp: %2.0ff</p>\
  </body>\
</html>",

   t
  );
  
  server.send(200, "text/html", htmlIndex);
}

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
static char myBuff[15];
static char humBuff[15];
int setTemp = 80;
char thermostatMode;
int tempDelta = 2;

// ############### Sensors and Temperature Computations

// DHT Sensor
const int DHTPin = 14;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

float getTemp() {
  return dht.readTemperature(true);
}

float getHumidity() {
  return dht.readHumidity();
}

float computeHeatIndex(){
  return dht.computeHeatIndex(getTemp(), getHumidity());
}

// ############# Temperature Control Logic

void evalTemp(float temp, int setTemp) {
  int buttonMode = 1;
  
  switch(buttonMode){
    case 1:
        if (temp > (setTemp + tempDelta))
          {
            Serial.println("cooling on | ");
            Serial.println("Current Temp: ");
            Serial.println(temp);
            Serial.println("Set Temp:");
            Serial.println(setTemp);
            Serial.println("Temp Delta: ");
            Serial.println(tempDelta);
            Serial.println("Delta Temp: ");
            int deltaTemp = setTemp - tempDelta;
            Serial.println(deltaTemp);
            coolOn();
          }
        else if (temp < (setTemp - tempDelta))
          {
            Serial.println("cooling off");
             Serial.println("cooling on | ");
            Serial.println("Current Temp: ");
            Serial.println(temp);
            Serial.println("Set Temp:");
            Serial.println(setTemp);
            Serial.println("Temp Delta: ");
            Serial.println(tempDelta);
            Serial.println("Delta Temp: ");
            int deltaTemp = setTemp - tempDelta;
            Serial.println(deltaTemp);
            coolOff();
          }
        break;
    case 0:
      Serial.println("heating");
            heatOn();
      break;
  }
  
   

}





// ############### Relay Control

//LinkSprite Relay
const int Relay1 = 16;
const int Relay2 = 12;
int buttonPushCounter = 0;
int lastButtonState = 0;
const int Button1 = 13;
int buttonPushCounter1 = 0;
int lastButtonState1 = 0;
const int Button2 = 15;


//  ############### Control Functions
void coolOn(){
 heatOff();
 digitalWrite(Relay1,HIGH);
 
}

void coolOff(){
  digitalWrite(Relay1,LOW);
}

void heatOn(){
 coolOff();
 digitalWrite(Relay2,HIGH);
}

void heatOff(){
  digitalWrite(Relay2,LOW);
}



// ############### LCD Stuff

SSD1306Brzo display(0x3c, 4, 5);

void drawLCD(float temp,float humidity, int setTemp) {
    display.clear();
 
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);

    // Convert float to char array   
    dtostrf(temp,6,2,celsiusTemp);

    // Add temp to LCD buffer
    sprintf(myBuff, "%.0ff",temp); 

    // Print Temperature to LCD buffer
    display.drawString(0, 0, myBuff);
      

    // Add Humidity to LCD buffer
    sprintf(humBuff,"%.0f%%",humidity); 

    // print Humidity to LCD buffer
    display.drawString(0, 17, humBuff);

    // Add Set temp to LCD buffer:
    sprintf(myBuff, "Set Temp is: %i", setTemp);
    display.drawString(0,29,myBuff);

    // Display LCD
    display.display();
}

// ############### SETUP 

void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  display.init();

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Button1, INPUT); 
  pinMode(Button2, INPUT);

  dht.begin();
  delay(10);

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Starting the web server
  server.on("/", HTTP_GET,handle_index);
  server.on("/coolOn",coolOn);
  server.on("/coolOff",coolOff);
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);

  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}




// ############### Main Loop

void loop() {

  server.handleClient();
  
  float t = getTemp();
  float h = getHumidity();
  
 
  // Increase Temperature
  int buttonState = digitalRead(Button1);
  
   if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      setTemp++;
    } 
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  lastButtonState = buttonState;

 // Decrease Temperature

 int buttonState1 = digitalRead(Button2);
  
   if (buttonState1 != lastButtonState1) {
    // if the state has changed, increment the counter
    if (buttonState1 == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter1++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter1);
      setTemp--;
    } 
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  lastButtonState1 = buttonState1;
  
  
  
  evalTemp(t,setTemp);
  
  drawLCD(t,h,setTemp);  
  
  
}
