#include <Wire.h> // Importamos la libreria de comunicación Wire.h (transmisión y recepción de datos por un solo cable)
#include <VL53L0X.h> // Importamos la libreria de los sensores de distancia 

VL53L0X sensor_1, sensor_2, sensor_3, sensor_4, sensor_5; // Creamos 5 objetos de tipo VL530X

const int xshuts[] ={12, 14, 15, 16, 17}; // Definimos los pines que nos serviran como xshut para cada sensor

unsigned long lastSensor_t = 0; // Definimos una variable que nos guardará el tiempo anterior 
int S_Interval = 100; // Definimos el intervalo del tiempo de espera entre cada lectura

void setup() {
  Serial.begin(57600); // Inicializamos la comunicación serial
  Wire.begin(); // Inicializamos la comunicación Wire

  for(int i = 0; i < 5; i++){

    pinMode(xshuts[i], OUTPUT); // Declaramos los pines xshut como salidas
    digitalWrite(xshuts[i], LOW); // Ponemos en estado bajo los pines xshut, esto se realiza para reiniciar el sensor a la dirección de memoria que tienen por defecto una vez que ya se haya corrido el codigo por primera vez
  
  }

  delay(10); 
  // Mandamos llamar la función para inicializar el cada sensor, mandando el objeto "sensor_x", el pin xshut que le corresponde, la posción de memoria que le asignaremos y su indice
  Inicializar_Sensor(sensor_1, xshuts[0], 0x30, 1);
  Inicializar_Sensor(sensor_2, xshuts[1], 0x31, 2);
  Inicializar_Sensor(sensor_3, xshuts[2], 0x32, 3);
  Inicializar_Sensor(sensor_4, xshuts[3], 0x33, 4);
  Inicializar_Sensor(sensor_5, xshuts[4], 0x34, 5);


}

void loop() {
  
  long currentMillis = millis(); // Inicializamos la función millis la cual activara un contador de tiempo al iniciar el programa 
  if(currentMillis - lastSensor_t >= S_Interval){ // Comparamos si el valor actual de tiempo menos el valor del tiempo pasado es mayor o igual al intervalo de espera 
    lastSensor_t = currentMillis; // Si la consición es verdadera, entonces el valor de tiempo pasado se le asignará el valor del tiempo actual
    leerYPublicar(); // mandamos llamar la función para leer e imprimir los datos
  }
  
}

void leerYPublicar(){
  // Se leen los datos de cada uno de los sensores en milimetros 
  uint16_t d1 = sensor_1.readRangeSingleMillimeters();
  uint16_t d2 = sensor_2.readRangeSingleMillimeters();
  uint16_t d3 = sensor_3.readRangeSingleMillimeters();
  uint16_t d4 = sensor_4.readRangeSingleMillimeters();
  uint16_t d5 = sensor_5.readRangeSingleMillimeters();

  // Se imprimen las distancias separadas por comas y con un formato limpio para poder procesarlo en ROS
  Serial.print("D:");
  Serial.print(d1); Serial.print(",");
  Serial.print(d2); Serial.print(",");
  Serial.print(d3); Serial.print(",");
  Serial.print(d4); Serial.print(",");
  Serial.println(d5);
}

void Inicializar_Sensor(VL53L0X &obj_sensor, int pin_xshut, uint8_t direccion, int indice){
  
  digitalWrite(pin_xshut, HIGH); // Se pone el pin en alto para encender el sensor 
  delay(10);
  if (!obj_sensor.init()) { Serial.print("Error en Sensor !"); Serial.print(indice); while (1); } // Se inicializa el sensor, en caso de un error se manda a imprimir un mensaje y el programa no hara nada
  
  obj_sensor.setAddress(direccion); // Se le agrega la nueva direccion de memoria al sensor  


}