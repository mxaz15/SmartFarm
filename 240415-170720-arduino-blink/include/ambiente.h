/*_____________________________________
Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
#define DHTTYPE    DHT21     // DHT 21 (AM2301)

See guide for details on sensor wiring and usage:
https://learn.adafruit.com/dht/overview

_________________________________________________*/

#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 14     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
#define DATA_SENSOR 3
#define NO_ERROR 0
#define ERROR_TEMPERATURA 1
#define ERROR_HUMEDAD   2
#define SENSOR_ERROR_IDX 2
#define SENSOR_TEMP_IDX 0
#define SENSOR_HUM_IDX 1
void ambiente_init(void);
int32_t ambiente_min_delay_sensor(void);
float* ambiente_get_info(void);

//Sensor's atributtes

