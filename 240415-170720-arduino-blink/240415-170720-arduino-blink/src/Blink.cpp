// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <ambiente.h>
//#include <DHT.h>
//#include <DHT_U.h>
#include <suelo.h>
//#define DHTPIN 14     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
#define S1_SENSOR_PIN A3

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
//define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

//DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
int error;


int suelo_s1; //soil humedity level of sensor 1
const int sensor_pin = A3;  /* Soil moisture sensor O/P pin */
float *data_sensor;

void setup() {
  Serial.begin(115200);
  // Initialize device.
  ambiente_init();
  // Set delay between sensor readings based on sensor details.
  delayMS = ambiente_min_delay_sensor()/ 1000;
}

void loop() {
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.

  data_sensor =  ambiente_get_info();

  if (data_sensor[SENSOR_ERROR_IDX] == NO_ERROR )
  {
    Serial.print(F("Temperature: "));
    Serial.print(data_sensor[0]);
    Serial.println(F("°C"));

    Serial.print(F("Humidity: "));
    Serial.print(data_sensor[1]);
    Serial.println(F("%"));
  }
  else{
    error = int(data_sensor[2]);
    switch (error)
    {
    case ERROR_TEMPERATURA:
        Serial.println(F("Error reading temperature!"));

      break;
    case ERROR_HUMEDAD:
                  Serial.println(F("Error reading humidity!"));
    break;
     
    default:
      Serial.println(F("Comunication error!"));
      break;

    }
    

  }
  ///sensor_analog = analogRead(sensor_pin);
  //_moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
   suelo_s1 = suelo_nivel(S1_SENSOR_PIN);
   
   Serial.print("H_Suelo = ");
  Serial.print(suelo_s1);  /* Print Temperature on the serial window */
  Serial.println("%");
}