#include <suelo.h>
#include <Adafruit_Sensor.h>



int suelo_nivel(int pin)
{
    int nivel = 0;
    int lectura = 0;
    lectura = analogRead(pin);
    nivel = ( 100 - ( (lectura/4095.00) * 100 ) );
    return nivel;

}