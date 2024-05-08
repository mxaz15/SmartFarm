#include <Adafruit_Sensor.h>
#include <ambiente.h>
#include <suelo.h>
#include <control.h>
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
  delayMS = ambiente_min_delay_sensor()/ 1000;
}

void loop() {
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