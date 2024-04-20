#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 14     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
#define DATA_SENSOR 3
#define NO_ERROR 0
#define ERROR_TEMPERATURA 1
#define ERROR_HUMEDAD   2
#define SENSOR_ERROR_IDX 2
void ambiente_init(void);
int32_t ambiente_min_delay_sensor(void);
float* ambiente_get_info(void);

//Sensor's atributtes

