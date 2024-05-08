#include <suelo.h>
#include <Adafruit_Sensor.h>


/*Recibe el pin en donde esta conectado el sensor de humedad y devuelve un valor en porcentaje*/
int suelo_nivel(int pin)
{
    int nivel = 0;
    int lectura = 0;
    lectura = analogRead(pin);
    nivel = ( 100 - ( (lectura/4095.00) * 100 ) );
    return nivel;

}