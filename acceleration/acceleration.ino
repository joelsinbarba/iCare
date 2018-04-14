/*
  Código escrito por Joel Giménez para el CareX
*/
// Las librerías I2Cdev y MPU6050 deben ser instaladas
#include "I2Cdev.h"
#include "MPU6050.h"

// La librería Arduino Wire es requerida si "I2Cdev I2CDEV_ARDUINO_WIRE implementation"
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

//#define Processing // Para mostrar los valores en un grafico de processing

MPU6050 acel; // Nombre del acelerómetro

int16_t ax, ay, az; //  Ejes del acelerómetro

int retraso = 100;   // Milisegundos entre cada lectura

float ace_x, ace_y , ace_z ; // Valores finales de aceleración

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0 , 0 , 0 , 0 , 0 , 0 , 0x00, 0x00, '\r', '\n' };

void setup()
{
  // Unirse al bus (La librería I2Cdev no hace esto automáticamente)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // Inicializar las comunicaciónes seriales:
  Serial.begin(9600);

  // Inicializar el dispositivo
  Serial.println("Inicializando dispositivos I2C...");
  acel.initialize();

  // Verificar la conexión de los componentes
  Serial.println("Comprobando conexion de componentes...");
  Serial.println(acel.testConnection() ? "Conexion exitosa" : "Conexion fallida");

  //Los siguientes 3 valores deben ser probados con un programa específico para calibrar el acelerómetro
  acel.setXAccelOffset(-1807); // Este es el error que extraemos a las lecturas en el eje x
  acel.setYAccelOffset(-1461); // Este es el error que extraemos a las lecturas en el eje y
  acel.setZAccelOffset(-144); //  Este es el error que extraemos a las lecturas en el eje z
  #ifdef Processing
   Serial.println("Enviando datos para Processing...");
  #endif
} 

void loop()
{
    
  acel.getAcceleration(&ax, &ay, &az); // Extraer valores del acelerómetro

  //Cargar dichos valores en diferentes variables
  int x = ax;
  delay(1);
  int y = ay;
  delay(1);
  int z = az;

  /*
    zero_G es la lectura que esperamos obtener del acelerómetro cuando no
    detecta aceleraciónes.  Sustraemos este valor a las lecturas para
    obtener valores más exactos.
  */
  float zero_G = 512.0;
  /*
    escala es el numero de unidades que esperamos que la lectura del sensor
    cambie cuando la aceleración en un eje cambia hasta 1G.
    Dividimos la lectura con la escala para obtener la aceleración en la unidad
    de medida "G".
  */
  float escala = 1682;

  ace_x = ((float)x - zero_G) / escala ;
  ace_y = ((float)y - zero_G) / escala ;
  ace_z = ((float)z - zero_G) / escala ;
  
    //Imprimir eje x
    Serial.print(" x : ");
    Serial.print(ace_x);
    Serial.print("\t");
    //Imprimir eje y
    Serial.print(" y : ");
    Serial.print(ace_y);
    Serial.print("\t");
    //Imprimir eje z
    Serial.print(" z : ");
    Serial.print((float)z / 6728);
    Serial.println("\t");
 

#ifdef Processing
  // display quaternion values in InvenSense Teapot demo format:
  teapotPacket[2] = ace_x;
  teapotPacket[3] = ace_y;
  teapotPacket[4] = ace_z;
  teapotPacket[5] =  ace_x;
  teapotPacket[6] = ace_y;
  teapotPacket[7] = ace_z;
  teapotPacket[8] = ace_x;
  teapotPacket[9] = ace_y;
  //Serial.write(teapotPacket, 14);
  teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
 #else
    /*Deducción de orientación según la comparación de valores en el eje x*/
    /*
    //Esta prueba comentada es una prueba con condición, no se utiliza para obtener
    //al menos una respuestaif ((ace_x) > 0 && (ace_x) < 0.5 )
    {
      Serial.print("Centrado"); Serial.print("        ");
    }
    */

  if ( ace_x > 4 )
  {
    Serial.print("Izquierda"); Serial.print("        ");
  }else if ( ace_x < -5 )
  {
    Serial.print("Derecha"); Serial.print("        ");
  }else {
    Serial.print("Centrado"); Serial.print("        ");
  }
  /*fin deducción eje x */

  //Deducción de orientación según la comparación de valores en el eje y*/
  /*
    //Esta prueba comentada es una prueba con condición, no se utiliza para obtener
    //al menos una respuesta
    if((ace_y) > 0 && (ace_y) < 0.5 )
    {
        Serial.println("  Centrado  ");
    }
  */
  if ( ace_y < -1 )
  {
    Serial.print("Frente  ");Serial.print("        ");
  } else if ( ace_y > 2 )
  {
    Serial.print("Atras   ");Serial.print("        ");
  } else {
    Serial.print("Centrado");Serial.print("        ");
  }
  /*fin deducción eje y */
  //Deducción de orientación según la comparación de valores en el eje z*/
  if ( ace_z < -10 )
  {
    Serial.println("Boca Abajo");
  } else if ( ace_z > -1 )
  {
    Serial.println("Boca Arriba");
  } else {
    Serial.println("Centrado");
  }
  /*fin deducción eje y */
#endif
  // Retraso hasta la siguiente lectura:
  delay(retraso);
}
