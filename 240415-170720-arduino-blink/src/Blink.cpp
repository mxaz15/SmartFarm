#include <Adafruit_Sensor.h>
#include <ambiente.h>
#include <suelo.h>
#include <control.h>
//WEBSERVWER START INCLUDES
#include <WiFi.h>
#include <Arduino.h>
#include <Wire.h>

//WEBSERVER INCLUDES
#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

//String convertion
#include <iostream>
#include <string>


//INCLUDES
//#define DEBUG_MAIN
#define DEBUG_HTML
#define S1_SENSOR_PIN A3

//START WEBSERVER
AsyncWebServer server(80);
const char* ssid = "TP-Link_8CD8";
const char* password = "wifi2024";
const char* PARAM_MESSAGE = "message";

const char* PARAM_INPUT_1 = "Temperatura_max";
const char* PARAM_INPUT_2 = "Temperatura_min";
const char* PARAM_INPUT_3 = "Humedad_max";
const char* PARAM_INPUT_4 = "Humedad_min";
const char* PARAM_INPUT_5 = "Suelo_max";
const char* PARAM_INPUT_6 = "Suelo_min";

//Var to storing value
String T_max;
String T_min;
String H_max;
String H_min;
String S_max;
String S_min;


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Temperatura Max: <input type="text" name="Temperatura_max">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Temperatura min: <input type="text" name="Temperatura_min">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Humedad max: <input type="text" name="Humedad_max">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Humedad min: <input type="text" name="Humedad_min">
    <input type="submit" value="Submit">
  </form><br>
   <form action="/get">
    Suelo max: <input type="text" name="Suelo_max">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Suelo min: <input type="text" name="Suelo_min">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

//END WEBSERVER

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

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


//START WEBSERVER
  Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      T_max = inputMessage;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      T_min = inputMessage;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      H_max = inputMessage;
    }
    else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;
      H_min = inputMessage;
    }
    else if (request->hasParam(PARAM_INPUT_5)) {
      inputMessage = request->getParam(PARAM_INPUT_5)->value();
      inputParam = PARAM_INPUT_5;
      S_max = inputMessage;
    }
    else if (request->hasParam(PARAM_INPUT_6)) {
      inputMessage = request->getParam(PARAM_INPUT_6)->value();
      inputParam = PARAM_INPUT_6;
      S_min = inputMessage;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });

    server.onNotFound(notFound);

    server.begin();

    //END WEBSERVER





}

void loop() {


  // Get temperature event and print its value.
  time_now = millis();

  if(time_now - time_refresh > delayMS)
  {
    time_refresh = time_now;

    data_dht =  ambiente_get_info();

    #ifdef DEBUG_HTML
    Serial.print("T_max ");
    Serial.println(T_max);
    Serial.print("T_min ");
    Serial.println(T_min);
    Serial.print("H_max ");
    Serial.println(H_max);
    Serial.print("H_mim ");
    Serial.println(H_min);
    Serial.print("S_max ");
    Serial.println(S_max);
    Serial.print("S_min ");
    Serial.println(S_min);
    #endif
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