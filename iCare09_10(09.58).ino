/*
  Cadigo escrito por Joel Giménez para el CareX
*/
// Las librerías I2Cdev y MPU6050 deben ser instaladas
#include "I2Cdev.h"
#include "MPU6050.h"

// La librería Arduino Wire es requerida si "I2Cdev I2CDEV_ARDUINO_WIRE implementation"
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

//#define Processing //Si está definido, se envían valores a Processing
//#define imprEjes; //Si está definido, se imprimen los valores de cada eje en el monitor de arduino
#define imprOri; //Si está definido, se imprime la orientación de cada eje

MPU6050 acel; // Nombre del acelerametro

int16_t ax, ay, az; //  Ejes del acelerametro

int retraso = 20;   // Milisegundos entre cada lectura

float ace_x, ace_y , ace_z ; // Valores finales de aceleracian

int ori_x,ori_y,ori_z; // Enteros que representan la orientación: 0 = Nulo , 1 = Negativo , 2 = Positivo
int ulti_x,ulti_y,ulti_z; // Enteros que guardan el valor de la última orientación para comparar con la actual 

int sacudidas = 0;
unsigned long tiempo = 0 ;


// Estructura Packet con formato teapot
uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0 , 0 , 0 , 0 , 0 , 0 , 0x00, 0x00, '\r', '\n' };

/////............PULSOMETRO...............///////////////

//  Variables
int pulsoPin = 0;                 // Cable de datos conectado a "analog pin 0"
int blinkPin = 13;                // Pin que enciende con cada latido
int efectoPin = 5;                  // Pin para el efecto del latido
int ritmoEfecto = 0;                 // Ritmo en el que el led debe hacer el efecto

// Variables volatiles
volatile int BPM;                   // int que contiene el valor de "Analog 0". Actualizado cada 2mS
volatile int Signal;                // Contiene el valor recibido sin depurar 
volatile int IBI = 600;             // int que contiene el intervalo de tiempo entre latidos
volatile boolean Pulse = false;     // "True" al detectar un latido. "False" cuando no es un latido. 
volatile boolean QS = false;        // "True" cuando el arduino recibe un latido.


static boolean serialVisual = false;   // "True" si necesitamos ver los datos en el monitor de arduino, "False" para no imprimir nada.

/////////////////............FIN PULSOMETRO...........//////////////////////

void setup()
{
  // Unirse al bus (La librería I2Cdev no hace esto automaticamente)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // Inicializar las comunicacianes seriales:
  Serial.begin(9600);
  
  // Inicializar el dispositivo
  Serial.println("Inicializando dispositivos I2C...");
  acel.initialize();

  // Verificar la conexian de los componentes
  Serial.println("Comprobando conexion de componentes...");
  Serial.println(acel.testConnection() ? "Conexion exitosa" : "Conexion fallida");

  //Los siguientes 3 valores deben ser probados con un programa específico para calibrar el acelerametro
  acel.setXAccelOffset(-1807); // Este es el error que extraemos a las lecturas en el eje x
  acel.setYAccelOffset(-1461); // Este es el error que extraemos a las lecturas en el eje y
  acel.setZAccelOffset(-144); //  Este es el error que extraemos a las lecturas en el eje z
  #ifdef Processing
   Serial.println("Enviando datos para Processing...");
  #else
    Serial.println("Processing no esta siendo utilizado...");
  #endif
  /////............PULSOMETRO...............///////////////
  pinMode(blinkPin,OUTPUT);         // Pin del led que enciende con cada latido
  pinMode(efectoPin,OUTPUT);          //  Pin del led late a la par de cada latido
  interruptSetup();                 // Leer la señal del sensor cada 2mS 
  /////............FIN PULSOMETRO...............///////////////

  //delay(8000);
} 

void loop()
{   
    tiempo = millis(); 
   //  Serial.print("Tiempo"); Serial.println(tiempo);
  acel.getAcceleration(&ax, &ay, &az); // Extraer valores del acelerametro

  //Cargar dichos valores en diferentes variables
  int x = ax;
  int y = ay;
  int z = az;

  /*
    zero_G es la lectura que esperamos obtener del acelerametro cuando no
    detecta aceleracianes.  Sustraemos este valor a las lecturas para
    obtener valores mas exactos.
  */
  float zero_G = 512.0;
  /*
    escala es el numero de unidades que esperamos que la lectura del sensor
    cambie cuando la aceleracian en un eje cambia hasta 1G.
    Dividimos la lectura con la escala para obtener la aceleracian en la unidad
    de medida "G".
  */
  float escala = 1682;

  ace_x = ((float)x - zero_G) / escala ;
  ace_y = ((float)y - zero_G) / escala ;
  ace_z = ((float)z - zero_G) / escala ;
  #ifdef imprEjes
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
    #endif
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
 #endif
 #ifdef imprOri
        /*Deduccion de orientacian segan la comparacian de valores en el eje x*/
         if ( ace_x > 4 )
          { 
                ori_x = 1;
              //  Serial.print("Izquierda"); Serial.print("        ");
          }else if ( ace_x < -5 )
          {       ori_x = 2;
              //  Serial.print("Derecha"); Serial.print("        ");
           }else {
                  ori_x = 0;
              //  Serial.print("Centrado"); Serial.print("        ");
           }
            /*fin Deduccion eje x */

            //Deduccion de orientacian segan la comparacian de valores en el eje y*/
            if ( ace_y < -1 )
            {
                ori_y = 2;
             // Serial.print("Frente  ");Serial.print("        ");
            } else if ( ace_y > 2 )
            {
               ori_y = 1;
             // Serial.print("Atras   ");Serial.print("        ");
            } else {
              ori_y = 0;
            //  Serial.print("Centrado");Serial.print("        ");
            }
            /*fin Deduccion eje y */
            //Deduccion de orientacian segan la comparacian de valores en el eje z*/
            if ( ace_z < -10 )
            { 
              ori_z = 1;
              //Serial.println("Boca Abajo");
            } else if ( ace_z > -1 )
            {
              ori_z = 2;
             // Serial.println("Boca Arriba");
            } else {
              ori_z = 0;
           //   Serial.println("Centrado");
            }
            /*fin Deduccion eje y */
#endif

//Alarma
if (ori_x != ulti_x ){
    sacudidas++;
   /*Serial.print("Cuidado!"); 
   Serial.print( "  Actual:  ");
    Serial.print(ori_x);
     Serial.print( "  Ultima: ");
      Serial.print( ulti_x); Serial.print( "  Sacudidas : ");*/
      Serial.println( sacudidas);
}
if (ori_y != ulti_y ){
    sacudidas++;
      Serial.println( sacudidas);
}
if (ori_z != ulti_z ){
    sacudidas++;
      Serial.println( sacudidas);
}

if(millis() - tiempo < 5000){
  Serial.print("Menor a 5 segundos!");  Serial.println( millis() - tiempo ); 
//shaking
if(sacudidas > 50 ){
  Serial.print("Oye tranquilo Viejo!");
}
}

 pulsoLoop();

   ulti_x = ori_x;
   ulti_y = ori_y;
   ulti_z = ori_z;
  
  if(tiempo >= 6000){
    sacudidas = 0;
    tiempo = millis();
  }
  
}
void pulsoLoop(){
    serialOutput() ;       
    
  if (QS == true){     // Un latido fue encontrado
                       // Los LPM han sido determinados
        ritmoEfecto = 255;         // Encendido del led
        serialOutputWhenBeatHappens();   // Se encontra un latido, enviar esto al monitor serial      
        QS = false;                      // resetear el boolean Qs para la siguiente vez  
  }
     
  ledFadeToBeat();                      //Efecto del led
}
void ledFadeToBeat(){
    ritmoEfecto -= 15;                         
    ritmoEfecto = constrain(ritmoEfecto,0,255);   //  Evitar que el ritmo sea negativo
    analogWrite(efectoPin,ritmoEfecto);          //  Producir el efecto
  }

