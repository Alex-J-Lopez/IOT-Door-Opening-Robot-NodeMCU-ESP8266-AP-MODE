#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal.h>

// Below you will need to use your own WIFI informaiton.

const char* ssid = "Room308"; //WIFI Name, WeMo will only connect to a 2.4GHz network.
const char* password = "notroom308"; //WIFI Password
LiquidCrystal lcd(1, 3, 2, 13, 12, 14);
//defining the pins and setting up the "server"
const int dirPin = 16; //Pin for direction of stepper motor
const int stepPin = 5; //Pin for stepping the motor forward
int upperLimit = 4;
int lowerLimit = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Setting up AP Mode");
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT); 
  pinMode(upperLimit, INPUT_PULLUP);
  pinMode(lowerLimit, INPUT_PULLUP);
  
  // Connect to WiFi network
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  delay(1000);
  lcd.clear();
  lcd.print("Use this IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.softAPIP());
  //Trying to connect it will display dots
 
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void openDoor(){
    while(digitalRead(lowerLimit) == HIGH){
        digitalWrite(dirPin, LOW);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
        yield();
      }
      delay(5000);
      while(digitalRead(upperLimit) == HIGH){
        digitalWrite(dirPin, HIGH);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
        yield();
      }
  }

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  //Match the request, checking to see what the currect state is
  int value = LOW;

  //cycle door
  
  if (request.indexOf("/DOOR=ON") != -1) {
      openDoor();
  } 


  //Build the html page
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<div align=\"center\">");
  client.println("<br><br><br><br><br><br><br>");

  client.println("<a href=\"/DOOR=ON\"><button><font size=12>CYCLE DOOR</button></a> <br><br>");

  client.println("</div>");
  
  client.println("</html>");
 
  delay(5000); //increased delay, as device was hanging after a few minutes at 1ms
  Serial.println("Client disconnected");
  Serial.println("");
 
}//END
