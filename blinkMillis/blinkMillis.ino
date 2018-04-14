//episode1: replacing delay() with millis ()

// --------CONSTANTS (won't change)---------------

const int vibraPin =  5;
const int vibraTiempo = 500; // number of millisecs that LED is on
const int vibraIntervalo = 300; // number of milisecs when the LED is off


//------------ VARIABLES (will change)---------------------

byte vibraEstado = HIGH; // this variable is necesary to help arduino monitor the status of LED
                            //the initial condition (t=0) can be either HIGH or LOW
unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long tiempoVibraPrevio = 0;   // will store last time the LED was updated


//========== THE SETUP ==============================

void setup() {
// put your setup code here, to run once:
  
  Serial.begin(9600); //default comm speed between arduino and serial monitor. no need to change.
  Serial.println("blinkMillis.ino");  // so we know what sketch is running
  pinMode(vibraPin, OUTPUT);
}

//========== THE LOOP ==============================

void loop() {
  // put your main code here, to run repeatedly:
  
  currentMillis = millis();   // capture the latest value of millis()
                              // this is equivalent to noting the time from a clock
  Vibracion();  //monitoring status of LED and do approproate actions

}

//------------ OTHER FUNCTIONS ---------------------

void Vibracion() {
  if (vibraEstado == LOW) { //if the LED is OFF
     Serial.println("LOW");
    if ((unsigned long) currentMillis - tiempoVibraPrevio >= vibraIntervalo) { 
      vibraEstado = HIGH;
      digitalWrite(vibraPin, vibraEstado);
      tiempoVibraPrevio += vibraIntervalo;
    }
  }
  else {  // i.e. if onBoardLedState is HIGH (if the LED is ON)
    Serial.println("HIGH");
    if ((unsigned long) currentMillis - tiempoVibraPrevio >= vibraTiempo) {
      vibraEstado = LOW;
      digitalWrite(vibraPin, vibraEstado);
      tiempoVibraPrevio += vibraTiempo;
    }
  }
}


