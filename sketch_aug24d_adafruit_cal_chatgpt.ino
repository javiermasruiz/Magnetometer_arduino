#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

//Sensor HMC5883L
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// Variables para calibración
float x_min = 1000, x_max = -1000;
float y_min = 1000, y_max = -1000;
float z_min = 1000, z_max = -1000;

// Duración de la calibración en milisegundos
const unsigned long CALIBRATION_TIME = 10000; // 10 segundos

void setup() {
  Serial.begin(9600);

  if (!mag.begin()) {
    Serial.println("Ooops, no se ha detectado un sensor HMC5883... Revisa tu conexión");
    while (1);
  }

  Serial.println("Iniciando calibración automática de 10 segundos...");
  Serial.println("Mueve el sensor en todas direcciones durante este tiempo.");

  unsigned long startTime = millis();
  sensors_event_t event;

  // Calibración: toma min y max de cada eje
  while (millis() - startTime < CALIBRATION_TIME) {
    mag.getEvent(&event);

    if (event.magnetic.x < x_min) x_min = event.magnetic.x;
    if (event.magnetic.x > x_max) x_max = event.magnetic.x;

    if (event.magnetic.y < y_min) y_min = event.magnetic.y;
    if (event.magnetic.y > y_max) y_max = event.magnetic.y;

    if (event.magnetic.z < z_min) z_min = event.magnetic.z;
    if (event.magnetic.z > z_max) z_max = event.magnetic.z;

    delay(50); // lectura cada 50 ms
  }

  Serial.println("Calibración completada.");
  Serial.print("X min/max: "); Serial.print(x_min); Serial.print(" / "); Serial.println(x_max);
  Serial.print("Y min/max: "); Serial.print(y_min); Serial.print(" / "); Serial.println(y_max);
  Serial.print("Z min/max: "); Serial.print(z_min); Serial.print(" / "); Serial.println(z_max);

  Serial.println("X,Y,Z"); // cabecera CSV
}

void loop() {
  sensors_event_t event;
  mag.getEvent(&event);

  // Aplica calibración: offset y escala
  float x_offset = (x_max + x_min) / 2;
  float y_offset = (y_max + y_min) / 2;
  float z_offset = (z_max + z_min) / 2;

  float x_scale = (x_max - x_min) / 2;
  float y_scale = (y_max - y_min) / 2;
  float z_scale = (z_max - z_min) / 2;

  float x_corrected = (event.magnetic.x - x_offset) / x_scale;
  float y_corrected = (event.magnetic.y - y_offset) / y_scale;
  float z_corrected = (event.magnetic.z - z_offset) / z_scale;

  // Imprime valores calibrados en formato CSV
  Serial.print(x_corrected); Serial.print(",");
  Serial.print(y_corrected); Serial.print(",");
  Serial.println(z_corrected);

  delay(500);
}
