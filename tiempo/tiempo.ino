//episode1: replacing delay() with millis ()

unsigned long millisActual = 0;    // stores the value of millis() in each iteration of loop()
unsigned long tiempoPrevio = 0;   // will store last time the LED was updated
const int vibraPin =  5;
byte vibraEstado = HIGH; // this variable is necesary to help arduino monitor the status of LED

unsigned long tiempoPrevio2 = 0;   // will store last time the LED was updated
const int vibraPin2 =  6;
byte vibraEstado2 = HIGH; // this variable is necesary to help arduino monitor the status of LED
//========== THE SETUP ==============================

void setup() {
// put your setup code here, to run once:
  
  Serial.begin(9600); //default comm speed between arduino and serial monitor. no need to change.
   pinMode(vibraPin, OUTPUT); //El vibrador es un output
}

//========== THE LOOP ==============================

void loop() {
  millisActual = millis();   // capture the latest value of millis()                        
  tiempo();
  tiempo2();
}

//------------ OTHER FUNCTIONS ---------------------//

void tiempo() {
    if ((unsigned long) millisActual - tiempoPrevio < 1000) {
      Serial.println("Aun no han pasado 2 segundos!");
      digitalWrite(vibraPin, vibraEstado);
    }else{
      Serial.println("Han pasado 2 segundos!");
      //digitalWrite(vibraPin, vibraEstado);
        vibraEstado = !vibraEstado;
      tiempoPrevio  += 1000;
    }
  }

  void tiempo2() {
    if ((unsigned long) millisActual - tiempoPrevio2 < 500) {
      Serial.println("Aun no han pasado 2 segundos!");
      digitalWrite(vibraPin2, vibraEstado2);
    }else{
      Serial.println("Han pasado 2 segundos!");
      //digitalWrite(vibraPin, vibraEstado);
        vibraEstado2 = !vibraEstado2;
      tiempoPrevio2  += 500;
    }
  }
