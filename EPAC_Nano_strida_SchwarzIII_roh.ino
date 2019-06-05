/*
  EPAC_Nano_StridaSchwarzIII                   Zirn 30.1.2019
  3 Fahrstufen, Freilauf (o. Bremse), vmax-Begrenzung, Anfahrhilfe
*/
#include <Servo.h>
Servo myServo;
// Eingangsbelegung
int led = 13; // Spannungsversorgung Abschaltautomatik
int tretein2 = 8;  // Hall-Sensor am Tretlager 
int tretein = 7;  // 2.Hall-Sensor am Tretlager 
int motein = 4;  // Hall-Sensor am Motor (vmax)
int tastein = 3;  // Daumentaster links
int tastein2 = 6;  // Daumentaster rechts
int bremsein = 10;  // Bremshebel (parallel)
// Zustandsvariable
int hallState = 0; // Hall-Sensor am Tretlager
int hallState_alt = 0;
int hallState2 = 0; // 2.Hall-Sensor am Tretlager
int motState = 0;  // Hall-Sensor am Motor
int motState_alt = 0;
int tastState = 0; // Daumentaster links
int tastState2 = 0; // Daumentaster rechts
int bremsState = 0; // Bremshebel
float zeit_alt;      // Zeitbasis
int zaehler =0;       // Zähler Tretkurbelflanken
int Richtung = 0;     // Richtung Tretkurbel
int zaehlerv =0;      // Zähler Motorflanken
int Sollwert = 90;    // Unterstützungssollwert
int angle = 90;        // Servowinkel 
float angle_k = 90;    // Zwischenwerte f. Tiefpass
float angle_k_1 = 90;  // float wg. Rundungsfehler
int angle_aus=90;          // Servowinkelausgabe alst integer
int vel = 0;          // Momentangeschwindigkeit
int loopzaehler = 0;  // Zähler für die Datenaufzeichnung

void setup() {   // digitale Eingänge setzen             
  pinMode(led, OUTPUT);
  pinMode(tretein, INPUT_PULLUP); 
  pinMode(tretein2, INPUT_PULLUP); 
  pinMode(tastein, INPUT_PULLUP);
  pinMode(tastein2, INPUT_PULLUP);
  pinMode(motein, INPUT_PULLUP);
  pinMode(bremsein, INPUT_PULLUP); 
  myServo.attach(9); myServo.write(angle_aus);
  digitalWrite(led, HIGH);
  Serial.begin(9600);
  delay(500);
}

void loop() {
   zeit_alt = millis();
   while ((millis() - zeit_alt) < 200){ // 200 ms Abtasten
     hallState = digitalRead(tretein); // Flankenzählen Tretkurbel 
     hallState2 = digitalRead(tretein2); // Richtungserkennung
     motState = digitalRead(motein); // Flankenzählen Motor
     if(hallState != hallState_alt){
       zaehler++;
       if(hallState2 == HIGH && (hallState > hallState_alt)){ Richtung = 0;}
       else if(hallState2 == LOW && (hallState < hallState_alt)){ Richtung = 0;}
       else { Richtung = 1;} // Auswertung 2. Hallsensor
       hallState_alt = hallState;
      }
     if(motState != motState_alt){
       zaehlerv++;
       motState_alt = motState;}
     delay(1);
     //Richtung = 1;  // zu Diagnosezwecken
   }  // of while 
  // Unterstützungsstufe wählen 
     tastState = digitalRead(tastein);   
     tastState2 = digitalRead(tastein2);
     bremsState = digitalRead(bremsein);
     if(tastState == LOW && tastState2 == HIGH && bremsState == HIGH) {Sollwert=137;} 
     else if(tastState2 == LOW && tastState == HIGH && bremsState == HIGH) {Sollwert=125;}  
     else if(tastState == LOW && tastState2 == LOW && bremsState == HIGH) {Sollwert = 155;}  
     else { Sollwert=90;}  
         
  // Geschwindigkeitsermittlung und Trethilfebegrenzung auf 25 km/h 
  vel=zaehlerv*0.29;
  zaehlerv = 0;      // Zähler zurücksetzen
  if (vel >= 25){Richtung = 0;}
  // Tretsensor mit Anfahrhilfe
   if (Richtung == 1){angle=Sollwert;}
   else {angle=90 + 0.25*(Sollwert-90);}
   // PT1-Tiefpass mit TF=2s genau mit float-Variablen 
   angle_k = (angle + 10*angle_k_1)/(1+10);
   angle_k_1 = angle_k;
   angle_aus=int(angle_k);    // float --> int
   myServo.write(angle_aus); // Übergabe an Servo
  // Serial-Ausgabe (opt.)
   Serial.print(loopzaehler);
   Serial.print(" ");
   Serial.print(angle_aus);
   Serial.print(" ");
   Serial.print(motState);
   Serial.print(" ");
   Serial.print(vel);
   Serial.print(" ");
   Serial.print(hallState);
   Serial.print(" ");
   Serial.print(hallState2);
   Serial.print(" ");
   Serial.print(tastState);
   Serial.print(" ");
   Serial.print(tastState2);
   Serial.print(" ");
   Serial.print(Richtung); 
   Serial.print(" ");
   Serial.print(bremsState);
   Serial.println(";");
   
   loopzaehler++;
   Richtung=0;
   zaehler=0;
}  // of loop
