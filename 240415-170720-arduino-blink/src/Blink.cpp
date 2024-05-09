#include <Adafruit_Sensor.h>
#include <ambiente.h>
#include <suelo.h>
#include <control.h>

//WEBSERVWER START INCLUDES
#include <WiFi.h>
#include <Arduino.h>
#include <Wire.h>


// Replace the X's with your network credentials
const char* ssid = "moto";
const char* password = "moto1234";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String relay1State = "off";
String relay2State = "off";

// Assign output variables to GPIO pins
//const int output26 = 26;
//const int output27 = 27;
const int Relay1 = 15;
const int Relay2 = 2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//WEBSERVWER ENDS INCLUDES


//INCLUDES
#define DEBUG_MAIN
#define S1_SENSOR_PIN A3

uint32_t delayMS;
int error;
int data_suelo; //soil humedity level of sensor 1
const int sensor_pin = A3;  /* Soil moisture sensor O/P pin */
float *data_dht;
uint32_t time_now = 0;
uint32_t time_refresh = 0;

void setup() {
  Serial.begin(115200);
  // Initialize device.
  ambiente_init();
  control_init_out();
  // Set delay between sensor readings based on sensor details.
  //delayMS = ambiente_min_delay_sensor()/ 1000;
  delayMS = 2000;
  //STAR WEBSERVER
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(Relay1, LOW);
  digitalWrite(Relay2, LOW);

    // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  //END WEBSERVER

}

void loop() {

//START WEBSERVER

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    #ifdef DEBUG_MAIN
    Serial.println("New Client.");          // print a message out in the serial port
    #endif
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              relay1State = "on";
              digitalWrite(Relay1, HIGH);

            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              relay1State = "off";
              digitalWrite(Relay1, LOW);

            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              relay2State = "on";
              digitalWrite(Relay2, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              relay2State = "off";
              digitalWrite(Relay2, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");



            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>Adapted by Osvaldo Cantone <a href=http://www.cantone.com.ar/wordpress/esp32-web-server-relay-control-lcd-i2c-display/>tecteach.net</a> </p>");

            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Relay 1 (GPIO 15) - State: " + relay1State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (relay1State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>Relay 2 (GPIO 2) - State: " + relay2State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (relay2State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }



  //END WEBSERVER
  

  // Get temperature event and print its value.
  time_now = millis();

  if(time_now - time_refresh > delayMS)
  {
    time_refresh = time_now;

    data_dht =  ambiente_get_info();

    if (data_dht[SENSOR_ERROR_IDX] == NO_ERROR )
    {
    #ifdef DEBUG_MAIN
    Serial.print(F("Temperature: "));
    Serial.print(data_dht[0]);
    Serial.println(F("°C"));
    Serial.print(F("Humidity: "));
    Serial.print(data_dht[1]);
    Serial.println(F("%"));
    #endif
   }
    else{
      error = int(data_dht[2]);
      switch (error)
      {
      case ERROR_TEMPERATURA:
        #ifdef DEBUG_MAIN
        Serial.println(F("Error reading temperature!"));
        #endif

      break;
      case ERROR_HUMEDAD:
      #ifdef DEBUG_MAIN
                  Serial.println(F("Error reading humidity!"));
      #endif
      break;
     
      default:

      #ifdef DEBUG_MAIN
      Serial.println(F("Comunication error!"));
      #endif

      break;

      }
    }
    
  

  
   
  
   data_suelo = suelo_nivel(S1_SENSOR_PIN);
  #ifdef DEBUG_MAIN
   Serial.print("H_Suelo = ");
   Serial.print(data_suelo);  /* Print Temperature on the serial window */
   Serial.println("%");
   #endif
   control_Temperatura(data_dht);
   control_Suelo(data_suelo);

   }


}