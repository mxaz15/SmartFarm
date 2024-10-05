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


//DEBUG DEFINES 
//#define DEBUG_MAIN
//#define DEBUG_HTML


#define S1_SENSOR_PIN A3

//START WEBSERVER
AsyncWebServer server(80);
//const char* ssid = "MarielSalinas";
//const char* password = "segfue9199";



//const char* ssid = "TP-Link_8CD8";
//const char* password = "wifi2024";

//const char* ssid = "moto";
//const char* password = "moto1234";

const char* ssid = "MQAZ 4928";
const char* password = "7B%b2027";


const char* PARAM_MESSAGE = "message";

const char* PARAM_INPUT_1 = "Temperatura_max";
const char* PARAM_INPUT_2 = "Temperatura_min";
const char* PARAM_INPUT_3 = "Humedad_max";
const char* PARAM_INPUT_4 = "Humedad_min";
const char* PARAM_INPUT_5 = "Suelo_max";
const char* PARAM_INPUT_6 = "Suelo_min";

//Var to storing value
int T_max_actual;
int T_min_actual;
int H_max_actual;
int H_min_actual;
int S_max_actual;
int S_min_actual;


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body bgcolor=96c8a2>

<style>
  .btn-group {
    display: flex;
    justify-content: center; /* Centra los botones horizontalmente */
    margin-bottom: 10px; /* Añade un poco de espacio entre los grupos de botones */
  }

  .btn-group button {
    background-color: #04AA6D; /* Fondo verde */
    border: 1px solid green; /* Borde verde */
    color: white; /* Texto blanco */
    padding: 10px 24px; /* Espaciado */
    cursor: pointer; /* Icono de mano */
    margin: 5px; /* Espaciado entre los botones */
  }

  /* Cambiar el color de fondo al pasar el mouse */
  .btn-group button:hover {
    background-color: #3e8e41;
  }
</style>

   <h1 align="center">AGRICULTURA INTELIGENTE</h1>
  <div class="btn-group" style="width:100%">
    <button style="width:10%" onclick="window.location.href='/new_page'">Valores Actuales Configrados</button>
  </div>
  <div class="btn-group" style="width:100%">
    
  <button style="width:10%" onclick="window.location.href='/actual_value'">Valores Actuales Sensados</button>
    
  </div>
  
 
  <form action="/get">
    <p align="center">Temperatura Maxima</p>
    <p align="center"><input type="text" name="Temperatura_max" autofocus placeholder="Introducir Valor Numerico"> <input type="submit" value="Ingresar"></p>
     </form><br>
     
  <form action="/get">
    <p align="center">Temperatura Minima</p>
    <p align="center"><input type="text" name="Temperatura_min" autofocus placeholder="Introducir Valor Numerico">
    <input type="submit" value="Ingresar"></p>
    </form><br>
    
  <form action="/get">
    <p align="center">Humedad Maxima</p>
    <p align="center"><input type="text" name="Humedad_max" autofocus placeholder="Introducir Valor Numerico">
    <input type="submit" value="Ingresar"></p>
    </form><br>
    
  <form action="/get">
    <p align="center"> Humedad Minima</p>
    <p align="center"><input type="text" name="Humedad_min" autofocus placeholder="Introducir Valor Numerico" >
    <input type="submit" value="Ingresar"></p>
    </form><br>
    
  <form action="/get">
    <p align="center">Nivel Maximo de Humedad en Suelo</p>
     <p align="center"><input type="text" name="Suelo_max" autofocus placeholder="Introducir Valor Numerico">
    <input type="submit" value="Ingresar"></p>
    </form><br>
    
  <form action="/get">
    <p align="center">Nivel Minimo de Humedad en Suelo</p> 
    <p align="center"> <input type="text" name="Suelo_min" autofocus placeholder="Introducir Valor Numerico">
    <input type="submit" value="Ingresar" ></p>
    </form><br>


</body></html>)rawliteral";


const char index_html_2[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body bgcolor=96c8a2>
  <h1 align="center">AGRICULTURA INTELIGENTE</h1></head>

  <body>
  <br><a href='/'>Volver a la pagina principal</a>
  </body></html>
)rawliteral";

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
    //request->send(200, "text/html", "<!DOCTYPE HTML><html><head><title>ESP Return Home </title>"" </head><body bgcolor=96c8a2>""<h1 align=\"center\">AGRICULTURA INTELIGENTE</h1>""TEMPERATURA MAXIMA " + T_max_actual +"<br><a href=\"/\">Volver a la pagina principal</a>");

  });

  //Send a request to the second page
  server.on("/new_page", HTTP_GET, [](AsyncWebServerRequest *request){
    String html_page = "<html>";
    html_page += "<!DOCTYPE HTML><html><head>";
    html_page += "<title>ESP Input Form</title>";
    html_page += "<style>";
    html_page += "p { background-color: #D3D3D3; border: 2px solid black; padding: 10px; margin: 20px auto; width: 50%; text-align: center; }";
    html_page += "</style>";
    html_page += "</head><body bgcolor=96c8a2>";
    html_page += "<h1 align=\"center\">AGRICULTURA INTELIGENTE</h1>";
    html_page += "<h1 align=\"center\">Valores Actuales Configrados</h1>";
    html_page += "<p>Temperatura maxima actual: " + String(T_max_actual) + "</p>";
    html_page += "<p>Temperatura minima actual: " + String(T_min_actual) + "</p>";
    html_page += "<p>Humedad maxima actual: " + String(H_max_actual) + "</p>";
    html_page += "<p>Humedad minima actual: " + String(H_min_actual) + "</p>";
    html_page += "<p>Humedad de suelo maxima actual: " + String(S_max_actual) + "</p>";
    html_page += "<p>Humedad de suelo minima actual: " + String(S_min_actual) + "</p>";
    html_page += "<br><a href='/'>Volver a la pagina principal</a>";
    html_page += "</body></html>";
    
    request->send(200, "text/html", html_page);
});

//Send a request to the second page
  server.on("/actual_value", HTTP_GET, [](AsyncWebServerRequest *request){
    String html_page = "<html>";
    html_page += "<!DOCTYPE HTML><html><head>";
    html_page += "<title>ESP Input Form</title>";
    html_page += "<style>";
    html_page += "p { background-color: #D3D3D3; border: 2px solid black; padding: 10px; margin: 20px auto; width: 50%; text-align: center; }";
    html_page += "</style>";
    html_page += "</head><body bgcolor=96c8a2>";
    html_page += "<h1 align=\"center\">AGRICULTURA INTELIGENTE</h1>";
    html_page += "<h1 align=\"center\">Valores Actuales Sensados</h1>";
    html_page += "<p>Temperatura actual: " + String(data_dht[SENSOR_TEMP_IDX]) + "</p>";
    html_page += "<p>Humedad actual: " + String(data_dht[SENSOR_HUM_IDX]) + "</p>";
    html_page += "<p>Nivel de suelo actual: " + String(data_suelo) + "</p>";
    html_page += "<br><a href='/'>Volver a la pagina principal</a>";
    html_page += "</body></html>";
    request->send(200, "text/html", html_page);
});


  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    int value;
    bool isint;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage> --- Thisi is the Max temeperature
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      isint = control_Check_int(inputMessage);
      value = inputMessage.toInt();
      if(isint !=false ){control_Set_T_max(value);}
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage> --- This is the Min temperature
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      isint = control_Check_int(inputMessage);
      value = inputMessage.toInt();
      if(isint !=false ){control_Set_T_min(value); }
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage> --- This is the Max humidity
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      isint = control_Check_int(inputMessage);
      value = inputMessage.toInt();
      if(isint !=false ){control_Set_H_max(value); }
    } // GET input4 value on <ESP_IP>/get?input3=<inputMessage> --- This is the Min humidity
    else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;
      isint = control_Check_int(inputMessage);
      value = inputMessage.toInt();
      if(isint !=false ){control_Set_H_min(value); }
    }// GET input5 value on <ESP_IP>/get?input3=<inputMessage> --- This is the Max soild humidity
    else if (request->hasParam(PARAM_INPUT_5)) {
      inputMessage = request->getParam(PARAM_INPUT_5)->value();
      inputParam = PARAM_INPUT_5;
      isint = control_Check_int(inputMessage);
      value = inputMessage.toInt();
      if(isint !=false ){control_Set_S_max(value);}
    }// GET input6 value on <ESP_IP>/get?input3=<inputMessage> --- This is the Min Soild humidity
    else if (request->hasParam(PARAM_INPUT_6)) {
      inputMessage = request->getParam(PARAM_INPUT_6)->value();
      inputParam = PARAM_INPUT_6;
      isint = control_Check_int(inputMessage);
      value = inputMessage.toInt();
      if(isint !=false ){control_Set_S_min(value); }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    #ifdef DEBUG_HTML
    Serial.println(inputMessage);
    #endif

    if(isint){
    request->send(200, "text/html", "<!DOCTYPE HTML><html><head><title>ESP Return Home </title>"" </head><body bgcolor=96c8a2>""<h1 align=\"center\">AGRICULTURA INTELIGENTE</h1>""El parametro " 
                                     + inputParam + " fue modificado, su valor actual es " + inputMessage +
                                     "<br><a href=\"/\">Volver a la pagina principal</a>");
    }
    else{
    request->send(200, "text/html", "<!DOCTYPE HTML><html><head><title>ESP Return Home </title>"" </head><body bgcolor=96c8a2>""<h1 align=\"center\">AGRICULTURA INTELIGENTE</h1>""<br>Valor no modificado, se ingreso " 
                                     + inputMessage +" y debe ingresar un numero" "<br>Por favor ingrese el valor nuevamente.""<br><a href=\"/\">Volver a la pagina principal</a>");

              }
                                     

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
    data_suelo = suelo_nivel(S1_SENSOR_PIN);


    //Varaibles to show in a second page.
    T_max_actual = control_Get_T_max();
    T_min_actual = control_Get_T_min();
    H_max_actual = control_Get_H_max();
    H_min_actual = control_Get_H_min();
    S_max_actual = control_Get_S_max();
    S_min_actual = control_Get_S_min();

    #ifdef DEBUG_HTML
    Serial.print("T_max ");
    Serial.println(control_Get_T_max());
    Serial.print("T_min ");
    Serial.println(control_Get_T_min());
    Serial.print("H_max ");
    Serial.println(control_Get_H_max());
    Serial.print("H_mim ");
    Serial.println(control_Get_H_min());
    Serial.print("S_max ");
    Serial.println(control_Get_S_max());
    Serial.print("S_min ");
    Serial.println(control_Get_S_min());
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
    
  

  
   
  //Take actuaL values and send to controler
  
  #ifdef DEBUG_MAIN
   Serial.print("H_Suelo = ");
   Serial.print(data_suelo);  /* Print Temperature on the serial window */
   Serial.println("%");
   #endif
   control_Temperatura(data_dht);
   control_Suelo(data_suelo);

   }


}