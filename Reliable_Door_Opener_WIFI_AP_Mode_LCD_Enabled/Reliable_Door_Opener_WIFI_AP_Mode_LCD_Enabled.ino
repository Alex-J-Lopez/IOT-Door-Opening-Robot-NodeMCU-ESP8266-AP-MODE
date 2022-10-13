#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal.h>


const char* ssid = "Room308"; //WIFIAP Name
const char* password = "notroom308"; //WIFI Password

//defining the pins
const int dirPin = 16; //Pin for direction of stepper motor
const int stepPin = 5; //Pin for stepping the motor forward
int upperLimit = 4;    //Pin for upper limit switch
int lowerLimit = 0;    //Pin for lower limit switch
LiquidCrystal lcd(1, 3, 2, 13, 12, 14); //Define the pins for the LCD display without I2C converter

//Set the webserver port as port 80
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10);

  // Print a message to the LCD stating that the system is setting up.
  lcd.begin(16, 2);
  lcd.print("Setting up AP Mode");

  //Set the pins into their respective modes.
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT); 
  pinMode(upperLimit, INPUT_PULLUP);
  pinMode(lowerLimit, INPUT_PULLUP);
  
  //Setting up the wireless access point.
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  delay(1000);

  //Display the information about IP to the user.
  lcd.clear();
  lcd.print("Use this IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.softAPIP());

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void openDoor(){
    while(digitalRead(lowerLimit) == HIGH){
        //Display that the door is opening
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Door Status");
        lcd.setCursor(0, 1);
        lcd.print("Opening");
        //Open the door
        digitalWrite(dirPin, LOW);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
        yield();
      }
      //Display that the door is open and count down from 5 until the door starts to close.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Door Status");
      lcd.setCursor(0, 1);
      lcd.print("Open");
      lcd.setCursor(4, 1);
      for(int i=5; i>0; i++){
        delay(1000);
        lcd.print(5-i);
      }
      while(digitalRead(upperLimit) == HIGH){
        //State that the door is closing
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Door Status");
        lcd.setCursor(0, 1);
        lcd.print("Closing");
        
        //Close the door
        digitalWrite(dirPin, HIGH);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
        yield();
      }
      //State that the door is closed for 3 seconds and then return to showing the IP address to use.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Door Status");
      lcd.setCursor(0, 1);
      lcd.print("Closed");
      delay(3000);
      lcd.clear();
      lcd.print("Use this IP:");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.softAPIP()); 
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
  
  if (request.indexOf("/DOORCYCLE") != -1) {
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

  client.println("<a href=\"/DOORCYCLE\"><button><font size=12>CYCLE DOOR</button></a> <br><br>");

  client.println("</div>");
  
  client.println("</html>");
 
  delay(10); //increased delay, as device was hanging after a few minutes at 1ms
  Serial.println("Client disconnected");
  Serial.println("");
 
}//END
