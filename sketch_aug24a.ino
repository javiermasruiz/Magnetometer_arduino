#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

//Asignación de un ID único al sensor
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void displaySensorDetails(void)
{
  //Datos de la Datasheet
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);                              //Nombre del sensor
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);                           //Version del Sensor
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);                         //ID del sensor
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");    //Rango máximo del Sensor
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");    //Rango mínimo del Sensor 
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");   //Resolución del Sensor
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) 
{
  //Inicio del monitor serial
  Serial.begin(9600);
  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  
  //inicialización del sensor
  if(!mag.begin())
  {
    //Si no se detecta el sensor se obtiene el siguiente mensaje 
    Serial.println("Ooops, no se ha detectado un sensor HMC5883... Revisa tu conexión");
    while(1);
  }
  
  //Se muestran los datos del Datasheet del Sensor
  displaySensorDetails();
}

void loop(void) 
{
  //Se obtiene un nuevo Evento del sensor
  sensors_event_t event; 
  mag.getEvent(&event);
 
  // Se muetsran los resultados en Micro Tesla (uT)
  Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");

  //Sostén el módulo con Z apuntando hacia arriba de manera que se pueda medir la trayectoria de X y Y
  //Calcula la trayectoria cuando el sensor este nivelado y luego corrige los signos del eje.
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  //Una vez que tengas la trayectoria agrega tu ángulo de declinación, que es el error del sensor en tu ubicación.
  //Si no conoces tu ángulo de declinación puedes encontrarlo aquí: http://www.magnetic-declination.com/
  //Debes convertir el ángulo en radianes para poder colocar el valor en las siguientes líneas.
  //Si no logras encontrar tu ángulo de declinación, comenta las siguientes dos líneas, tu brújula estará ligeramente desviada.
  float declinationAngle = 0.06;
  heading += declinationAngle;
  
  //Corrige si los signos están invertidos
  if(heading < 0)
    heading += 2*PI;
    
  //Comprueba si hay wrap debido a la adición de declinación.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  //Convierta radianes a grados para facilitar la lectura
  float headingDegrees = heading * 180/M_PI; 
  
  Serial.print("Heading (degrees): "); Serial.println(headingDegrees);
  
  delay(500);
}