int A = 0;
int AA = 1;
int AAA= 2;// set the input pin for the potentiometer
#include <NewPing.h>
int low = 0;
int mid=0;
int high=0;// variable to store the value coming from the sensor
int pump=13;
int valve=9;
int frag=0;
int addr=0;

#include <EEPROM.h>
#define TRIGGER_PIN  10 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN    11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
byte buzzer    = 8;
float flowRate;
unsigned int flowMilliLitres;
volatile byte pulseCount; 
unsigned long oldTime;
float calibrationFactor = 4.5;
void setup() {
  // declare the ledPin as an OUTPUT:
   Serial.begin(9600);  
   pinMode(pump, OUTPUT);
   pinMode(valve, OUTPUT);
   digitalWrite(valve,HIGH);
  pinMode(sensorPin, INPUT);
pinMode(buzzer, OUTPUT);

pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
   oldTime           = 0;
   attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

}
 
void loop() {
  
  // read the value from the sensor:
  tank();
  leak();
  
int waterout=EEPROM.read(addr); 
  if(waterout==1){
    digitalWrite(valve,LOW);
  }else{
     digitalWrite(valve,HIGH);
  }
  distance();
   notify();
     
  delay(1000);          
                   
}
void distance(){
  float distance=sonar.ping_cm();
  Serial.println(distance);
  if(distance<=50){
     EEPROM.write(addr,1);
    
    Serial.println("water flowing");
  }else{
    EEPROM.write(addr,0);
     Serial.println("no flowing");
  }
  
}
void tank(){
  low = analogRead(A);
  mid= analogRead(AA);
  high= analogRead(AAA);
   Serial.println(low);
    Serial.println(mid);
     Serial.println(high);
     
       if((low>900)&&(mid>900) && (high>900))
       {
         Serial.println("sensor = 0%" );
         digitalWrite(pump,LOW);
                              
       }
       
        else if((low<900)&&(mid>900) && (high>900))
       {
         Serial.println("sensor = 33%" );
          distance();                      
       }
       
         else if((low<900)&&(mid<900) && (high>900))
       {
         Serial.println("sensor = 66%" );  
                              
       }
       
        else if((low<900)&&(mid<900) && (high<830))
       {
         Serial.println("sensor = 100%" ); 
         digitalWrite(pump,HIGH); 
                             
       }
}
void leak(){
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
     oldTime = millis();
     flowMilliLitres = (flowRate / 60) * 1000;
     Serial.println("  Current Liquid Flowing: ");             // Output separator
    Serial.println(flowMilliLitres);
    Serial.println("mL/Sec");
     if(flowMilliLitres>0 )
     {
     frag=1;;
     }else{
     frag=0;
       }
     pulseCount = 0;
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);     
  }
}
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
void notify(){
   
  int buzz=frag;
   if(buzz==1 && digitalRead(valve)== HIGH )
  {
    digitalWrite(buzzer,HIGH);
         delay(500);
         digitalWrite(buzzer,LOW);
         delay(500);
         Serial.println("leak");
  } else{
     digitalWrite(buzzer,LOW);
     Serial.println("no leak");
  }
}



