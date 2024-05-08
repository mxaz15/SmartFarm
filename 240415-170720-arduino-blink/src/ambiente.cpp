#include <ambiente.h>
#include <Adafruit_Sensor.h>
sensor_t sensor;
sensors_event_t event;
DHT_Unified dht(DHTPIN, DHTTYPE);
float sensor_info[DATA_SENSOR];



void ambiente_init(void)
{
    dht.begin();
    Serial.println(F("DHTxx Unified Sensor Example"));
    // Print temperature sensor details.
    dht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.println(F("------------------------------------"));

}

/*Devuelve un  array con el valor de temperatura y humedad*/
float* ambiente_get_info(void)
{

    dht.temperature().getEvent(&event);
    sensor_info[2] = NO_ERROR;
    if (isnan(event.temperature)) {
    //Serial.println(F("Error reading temperature!"));
    sensor_info[2] = ERROR_TEMPERATURA;
    }
    else {
    //Serial.print(F("Temperature: "));
   // Serial.print(event.temperature);
   // Serial.println(F("°C"));
    sensor_info[0] = event.temperature;
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
    //Serial.println(F("Error reading humidity!"));
    sensor_info[2] = ERROR_HUMEDAD;
    }
    else {
    //Serial.print(F("Humidity: "));
    //Serial.print(event.relative_humidity);
    //Serial.println(F("%"));
    sensor_info[1] = event.relative_humidity;
    }

    return sensor_info;
}


int32_t ambiente_min_delay_sensor()
{
    return sensor.min_delay;
}
