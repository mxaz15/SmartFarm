#include <control.h>
#include <ambiente.h>
#define DEBUG_CONTROL
#define CONTROL_MAX_TEMP 26
#define CONTROL_MIN_TEMP 20
#define CONTROL_MAX_HUM 30
#define CONTROL_MIN_HUM 20
#define CONTROL_MIN_H_SUELO 40
#define CONTROL_MAX_H_SUELO 70
//FAN
#define PIN_FAN 33
#define ENCEDER_FAN 1
#define APAGAR_FAN 0
//PUMP
#define PIN_PUMP 25
#define ENCEDER_PUMP 1
#define APAGAR_PUMP 0

//HUMEDITY
#define PIN_HUMEDAD 32
#define ENCENDER_PIN_HUM 1
#define APAGAR_PIN_HUM 0


void control_init_out(void)
{
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_HUMEDAD, OUTPUT);

}

/*recibe un puntero con los datos de temperatura y humedad habilita las salidas para regular las magnitudes*/
void control_Temperatura(float* p_data)
{
    //Control de temperatura
    if( p_data[SENSOR_TEMP_IDX] > CONTROL_MAX_TEMP )
    {
        digitalWrite(PIN_FAN, ENCEDER_FAN);
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Encender FAN_T");
        #endif
    }else{
        digitalWrite(PIN_FAN, APAGAR_FAN);
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Apagar fan_T");
        #endif
    }

    //Control de humedad
    if( p_data[SENSOR_HUM_IDX] > CONTROL_MAX_HUM )
    {
        digitalWrite(PIN_HUMEDAD, ENCENDER_PIN_HUM);
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Encender fan_H");
        #endif
    }else{
        digitalWrite(PIN_HUMEDAD, APAGAR_PIN_HUM);
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Apagar fan_H");
        #endif
    }
}
/*Recibe el valor porcentual de humedad en el suelo  y habilita/deshabilita el PIN_PUMP que regula la cantidad de agua */
void control_Suelo(int hum_suelo)
{
    if( hum_suelo < CONTROL_MIN_H_SUELO )
    {
        digitalWrite(PIN_PUMP, ENCEDER_PUMP);
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Encender bomba");
        #endif
    }else{
        digitalWrite(PIN_PUMP, APAGAR_PUMP);
        #ifdef DEBUG_CONTROL
        Serial.println("DEBUG: Apagar bomba");
        #endif
    }
}