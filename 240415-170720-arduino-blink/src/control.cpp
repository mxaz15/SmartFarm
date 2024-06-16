#include <control.h>
#include <ambiente.h>
//#define DEBUG_CONTROL
#define CONTROL_MAX_TEMP 22
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

//VAR TO STORING VALUE RANGE
int control_T_max = 20;
int control_T_min = 20;
int control_H_max = 50;
int control_H_min = 40;
int control_S_max = 60;
int control_S_min = 30;

//VAR ALARMS
bool control_alm_T = false;
bool control_alm_H = false;
bool control_alm_S = false;



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
    if( p_data[SENSOR_TEMP_IDX] > control_T_max )
    {
        digitalWrite(PIN_FAN, ENCEDER_FAN);
        control_alm_T = true;
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Encender FAN_T");
        #endif
    }else if(p_data[SENSOR_TEMP_IDX] < control_T_min) 
    {
        digitalWrite(PIN_FAN, APAGAR_FAN);
        control_alm_T = true;
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Apagar fan_T");
        #endif
    }
    else
    {
        digitalWrite(PIN_FAN, APAGAR_FAN);
        control_alm_T = false;
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Temperatura ok");
        #endif
    }

    //Control de humedad
    if( p_data[SENSOR_HUM_IDX] > control_H_max )
    {
        digitalWrite(PIN_HUMEDAD, ENCENDER_PIN_HUM);
        control_alm_H = true;
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Encender fan_H");
        #endif
    }else if (p_data[SENSOR_HUM_IDX] < control_H_min)
    {
        digitalWrite(PIN_HUMEDAD, APAGAR_PIN_HUM);
        control_alm_H = true;         
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: ALARMA de humedad minima");
        #endif
    }
    else 
    {
        
        digitalWrite(PIN_HUMEDAD, APAGAR_PIN_HUM);
        control_alm_H = false;
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Humedad ok");
        #endif
        

    }
}
/*Recibe el valor porcentual de humedad en el suelo  y habilita/deshabilita el PIN_PUMP que regula la cantidad de agua */
void control_Suelo(int hum_suelo)
{
    if( hum_suelo < control_S_min )
    {
        digitalWrite(PIN_PUMP, ENCEDER_PUMP);
        control_alm_S = true;
        #ifdef DEBUG_CONTROL
			        Serial.println("DEBUG: Encender bomba");
        #endif
    }
    else if (hum_suelo > control_S_max )
    {
        digitalWrite(PIN_PUMP, APAGAR_PUMP);
        control_alm_S = true;
        #ifdef DEBUG_CONTROL
        Serial.println("DEBUG: Apagar bomba");
        #endif
    }
    else{
        digitalWrite(PIN_PUMP,APAGAR_PUMP);
        control_alm_S = false;
        #ifdef DEBUG_CONTROL
		Serial.println("DEBUG:Nivel S ok");
        #endif

    }
}

//GETS FUNCTIONS

int control_Get_T_max (void){
    return control_T_max;

}

int control_Get_T_min(void)
{
    return control_T_min;
}

int control_Get_H_max(void)
{
    return control_H_max;
}
int control_Get_H_min(void)
{
    return control_H_min;
}
int control_Get_S_max(void)
{
    return control_S_max;
}
int control_Get_S_min(void)
{
    return control_S_min;
}

//SETS FUNCTION

void control_Set_T_max(int value)
{
    control_T_max = value;
}
void control_Set_T_min(int value)
{
     control_T_min = value;
}
void control_Set_H_max(int value)
{
    control_H_max = value;
}
void control_Set_H_min(int value)
{
    control_H_min = value;
}
void control_Set_S_max(int value)
{
    control_S_max = value;
}
void control_Set_S_min(int value)
{
    control_S_min = value;
}