#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math
#include <DHT.h>
#include <PulseSensorPlayground.h>  
#include <SoftwareSerial.h> // for bluetooth communication

#define DHTPIN 7  // pin for temperature and humidity     
#define DHTTYPE DHT11   

const int PULSE_SENSOR_PIN = 0;
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore

DHT dht(DHTPIN, DHTTYPE);
PulseSensorPlayground pulseSensor;
SoftwareSerial Bluetooth(2,4); // bluetooth serial
String msg,cmd;
int isAvailable = 0;

void setup() {
  Serial.begin(9600);
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.setThreshold(Threshold);
  dht.begin();
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor object created!");
  }
  
  Bluetooth.begin(9600);

  //health monitor 
  pinMode(10,INPUT);
  pinMode(11,INPUT);
}

void loop() {
  
  if(Bluetooth.available()){
    msg=Bluetooth.readString();
    isAvailable = 1; // in order to send it continously 
  }
  // humidity, temperature, ecg, beat per minute
  if (isAvailable > 0)
  {
    get_temperature();
    //get_health_ecg();
    get_BPM();
  }
  // send data at the interval of 10 seconds
  delay(10000);
}


void get_health_ecg(){
  if((digitalRead(10) == 1) || (digitalRead(11) == 1)){
    Serial.println("No value!");
  }
  else
  {
    Serial.print("ECG value:");
    Serial.println(analogRead(A1));
    Bluetooth.write(analogRead(A1));
  }
}

void get_temperature(){
  char buff[20];
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Serial.print("Temp:");
  Serial.println(t);

  Serial.print("Humidity:");
  Serial.println(h);

  sprintf(buff,"%d;%d;",h,t);
  Bluetooth.write(buff);
}

void get_BPM()
{
  int myBPM = pulseSensor.getBeatsPerMinute();
  if (pulseSensor.sawStartOfBeat()) {            
    // Constantly test to see if a beat happened
    Serial.println("♥  A HeartBeat Happened ! "); // If true, print a message
    Serial.print("BPM: ");
    Serial.println(myBPM);                        // Print the BPM value
    Bluetooth.write(myBPM);  
  }
  delay(20);
}
