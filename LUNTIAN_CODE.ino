// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLTmOH9p73"
#define BLYNK_DEVICE_NAME "BlynkTest"
#define BLYNK_AUTH_TOKEN "gDBhWS7e-uszW3xbzjgklrXdfePyRvUy"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>


//Default Zero and Default Label

int DEFAULT_ZERO = 0; 
String DEFAULT_LABEL = "N/A";

// Temperature & Humidity
#include <DHT.h>

#define DHTPIN 8          // Mention the digital pin where you connected 
#define DHTTYPE DHT22    // DHT 22  
DHT dht(DHTPIN, DHTTYPE);

#define ATOMIZER 3 //I-assign ang pin 3 sa Humidifier
#define HUMID_FAN 2 //I-assign ang pin 2 sa Fan Circuit
#define TEMP_FAN1 5 //fan1 assigned to pin 4 (intake)
#define TEMP_FAN2 12 //fan2 assigned to pin 5 (exhaust)

//Para sa threshold values for humidity
int OPTIMUM_TEMPERATURE;
int OPTIMUM_HUMIDITY;
int opt_temp;
int opt_humid;

#define trigPin1 9
#define echoPin1 10


//system water level
#define trigPin2 24
#define echoPin2 25 

#define far 19
#define near 2

long duration1;
int distanceCm1;



//Soil Management Subsystem
#define WATER_PUMP1 7 

int OPTIMUM_SOIL_MOISTURE_LEVEL;
int opt_smoist;

int OPTIMUM_WATER_LEVEL;
int opt_water;

int Pin1 = A1;

const int dry1 = 760; //value for dry sensor1
const int wet1 = 515; //value for wet sensor1

float sensor1Value = 0;

//light value
int OPTIMUM_LIGHT_INTENSITY;
int OPTIMUM_PHOTOPERIOD;
int opt_lux;
int opt_photo;
int TOLERANCE_LIGHT_INTENSITY = 30;
int liwanag;

//miscellaneous
String vegetable_choice;
int conf_deact;

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SKYFiber_MESH_828C";
char pass[] = "531061772";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

#include <TimeLib.h>


#include<Wire.h>

int BH1750address = 0x23;
byte buff[2];

const int Light = 13;

int ilawValue = 0;

int LED = A3;


BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
}

//climate management subsystem
void sendSensor() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(100);
  // Sets the trigPin1 on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin1, LOW);
  // Reads the echoPin1, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin1, HIGH);
  // Calculating the distance
  int distance_cm = duration1 * 0.034/2;
  int distance_percent = map(distance_cm, near, far, 100, 0);  
  
  // DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    Blynk.virtualWrite(V20, String("Error")); //Intake Error
    Blynk.virtualWrite(V21, String("Error")); //Exhaust Error
    Blynk.virtualWrite(V22, String("Error")); //Humidifier Error
    Blynk.virtualWrite(V35, String("Error")); //Humidifier Fan Error
    return;
  }

  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  
  Serial.print("Temperature : ");
  Serial.print(t);
  Serial.print("    Humidity : ");
  Serial.println(h);
  
  if (t > OPTIMUM_TEMPERATURE) {
    Serial.print("Intake fan ON! \n"); //Intake fan is OFF
    Serial.print("Exhaust fan ON! \n"); //Exhaust fan is ON
    analogWrite(TEMP_FAN1, 255);
    analogWrite(TEMP_FAN2, 255);
    Blynk.virtualWrite(V20, String("ON")); 
    Blynk.virtualWrite(V21, String("ON"));
}

  else if (t < OPTIMUM_TEMPERATURE) {
    Serial.print("Intake fan OFF! \n"); //Intake fan is ON
    Serial.print("Exhaust OFF! \n"); //Exhaust fan is OFF
    analogWrite(TEMP_FAN1, 0);
    analogWrite(TEMP_FAN2, 0);
    Blynk.virtualWrite(V20, String("OFF")); 
    Blynk.virtualWrite(V21, String("OFF"));
}


  if(distance_percent > OPTIMUM_WATER_LEVEL) {
    Serial.print("Humidifier's Water Level: ");
    Serial.print(distance_percent);
    Serial.print("% \n");
    Blynk.virtualWrite(V4, distance_percent);
  
    if (h > OPTIMUM_HUMIDITY) {
      Serial.print("Humidifier OFF! \n"); //OFF ang actuator
      Serial.print("Fan OFF! \n"); //OFF ang fan
      digitalWrite(HUMID_FAN, LOW);
      digitalWrite(ATOMIZER, LOW);
      Blynk.virtualWrite(V22, String("OFF")); //Humidifier OFF
      Blynk.virtualWrite(V35, String("OFF")); //Humidifier Fan OFF
    }

    else if (h < OPTIMUM_HUMIDITY) {
      Serial.print("Humidifier ON! \n"); //ON ang actuator
      Serial.print("Fan ON! \n"); //ON ang fan
      digitalWrite(HUMID_FAN, HIGH);
      digitalWrite(ATOMIZER, HIGH);
      Blynk.virtualWrite(V22, String("ON")); //Humidifier ON
      Blynk.virtualWrite(V35, String("ON")); //Humidifier Fan ON
    }
  }
  
  if(distance_percent <= OPTIMUM_WATER_LEVEL) {
    Serial.print("Humidifier's Water Level: ");
    Serial.print(distance_percent);
    Serial.print("% \n");
    Blynk.virtualWrite(V4, distance_percent);
    Serial.print("Message: Fill the humidifier with container! \n"); //Email device
    Serial.print("Humidifier OFF! \n"); //OFF ang actuator
    digitalWrite(HUMID_FAN, LOW); //OFF ang fan
    digitalWrite(ATOMIZER, LOW); //OFF ang humidifier
    Blynk.virtualWrite(V22, String("OFF")); //Humidifier OFF
    Blynk.virtualWrite(V35, String("OFF")); //Humidifier Fan OFF
    Blynk.logEvent("humidifiers_water_level","Humidifier's water supply is low. Please fill the humidifier's water tank immediately");
   }

}


void soil_subsystem(){
  
  int sensor1Value = analogRead(Pin1);

  //Sensors have a range of 515 to 760
  //We want to translate this to a scale or 0% to 100%

    int percentageHumidity1 = map(sensor1Value, wet1, dry1, 100, 0);
  
    percentageHumidity1 = abs(percentageHumidity1);
    
  Blynk.virtualWrite(V6, percentageHumidity1);
  Serial.print("Soil Moisture Level 1: ");
  Serial.print(percentageHumidity1);
  Serial.println("%"); 

  if (percentageHumidity1 >= OPTIMUM_SOIL_MOISTURE_LEVEL){
    digitalWrite(WATER_PUMP1, LOW);
    Serial.println("Water pump1 is off");
    Blynk.virtualWrite(V23, String("OFF")); //Water Pump 1 OFF
  }
  
  else if (percentageHumidity1 < OPTIMUM_SOIL_MOISTURE_LEVEL){
    digitalWrite(WATER_PUMP1, HIGH);
    Serial.println("Water pump1 is on"); 
    Blynk.virtualWrite(V23, String("ON")); //Water Pump 1 ON
 
  }
  
}


void water_level(){
  
  const int ledPin = 5;

  long duration;
  int distanceCm;
  
  // Clears the trigPin2
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(100);
  // Sets the trigPin2 on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin2, LOW);
  // Reads the echoPin2, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin2, HIGH);
  // Calculating the distance
  distanceCm = duration * 0.034/2;
  int percentageWaterLevel = map(distanceCm, near, far, 100,0);
  if (percentageWaterLevel <= OPTIMUM_WATER_LEVEL) {
    digitalWrite(ledPin, HIGH);
    Serial.print("System's Water Level: ");
    Serial.print(percentageWaterLevel);
    Serial.println("%");
    Blynk.logEvent("systems_water_level","System's water supply is low. Please fill the system's water tank immediately");
  }

   
  else {
      if (percentageWaterLevel > 100){
      Serial.println(distanceCm);
      Serial.print("System's Water Level: ");
      Serial.println("100%");
      }
    digitalWrite(ledPin, LOW);
  }
  
  // Prints the distance on the Serial Monitor
  Blynk.virtualWrite(V3, percentageWaterLevel);
  Serial.print("System's Water Level: ");
  Serial.print(percentageWaterLevel );
  Serial.println("%");
  
}

//light managemenet subsystem

bool led_set[2];
long timer_start_set[2] = {0xFFFF, 0xFFFF};
long timer_stop_set[2] = {0xFFFF, 0xFFFF};
unsigned char weekday_set[2];

long rtc_sec;
unsigned char day_of_week;

bool led_status[2];
bool update_blynk_status[2];
bool led_timer_on_set[2];

// #########################################################################################################
// LED 1


BLYNK_WRITE(V9)
{
  int val = param.asInt();

  if ( led_timer_on_set[0] == 0 )
    led_set[0] = val;
  else
    update_blynk_status[0] = 1;
}

// #########################################################################################################
// Timer 1
BLYNK_WRITE(V11) 
{
  unsigned char week_day;
 
  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime() ) 
  {
    timer_start_set[0] = (t.getStartHour() * 60 * 60) + (t.getStartMinute() * 60) + t.getStartSecond();
    timer_stop_set[0] = (t.getStopHour() * 60 * 60) + (t.getStopMinute() * 60) + t.getStopSecond();
    
    Serial.println(String("Start Time: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
    Blynk.virtualWrite(V46, String(t.getStartHour()) + ":" + t.getStartMinute() + ":" + t.getStartSecond());
    Blynk.virtualWrite(V47, String(t.getStopHour()) + ":" + t.getStopMinute() + ":" + t.getStopSecond());
                       
    Serial.println(String("Stop Time: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
                   
    for (int i = 1; i <= 7; i++) 
    {
      if (t.isWeekdaySelected(i)) 
      {
        week_day |= (0x01 << (i-1));
        Serial.println(String("Day ") + i + " is selected");
      }
      else
      {
        week_day &= (~(0x01 << (i-1)));
      }
    } 

    weekday_set[0] = week_day;
  }
  else
  {
    timer_start_set[0] = 0xFFFF;
    timer_stop_set[0] = 0xFFFF;
  }
}

// #########################################################################################################
BLYNK_WRITE(InternalPinRTC) 
{
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  unsigned long blynkTime = param.asLong(); 
  
  if (blynkTime >= DEFAULT_TIME) 
  {
    setTime(blynkTime);

    day_of_week = weekday();
  
    if ( day_of_week == 1 )
      day_of_week = 7;
    else
      day_of_week -= 1; 
    
    rtc_sec = (hour()*60*60) + (minute()*60) + second();
   
    Serial.println(blynkTime);
    Serial.println(String("RTC Server: ") + hour() + ":" + minute() + ":" + second());
    Serial.println(String("Day of Week: ") + weekday()); 
    Blynk.virtualWrite(V11, String (hour()) + ":" + minute() + ":" + second());
    
  }
}

// #########################################################################################################
BLYNK_CONNECTED() 
{
  Blynk.sendInternal("rtc", "sync"); 
}

// #########################################################################################################
void checkTime() 
{
  Blynk.sendInternal("rtc", "sync"); 
}

// #########################################################################################################
void led_mng()
{
  bool time_set_overflow;
  bool led_status_buf[2];
  
  for (int i=0; i<2; i++)
  {
    led_status_buf[i] = led_status[i];
    time_set_overflow = 0;
    
    if ( timer_start_set[i] != 0xFFFF && timer_stop_set[i] != 0xFFFF)
    {
      if ( timer_stop_set[i] < timer_start_set[i] ) time_set_overflow = 1;

      if ((((time_set_overflow == 0 && (rtc_sec >= timer_start_set[i]) && (rtc_sec < timer_stop_set[i])) ||
        (time_set_overflow  && ((rtc_sec >= timer_start_set[i]) || (rtc_sec < timer_stop_set[i])))) && 
        (weekday_set[i] == 0x00 || (weekday_set[i] & (0x01 << (day_of_week - 1) )))) )
        {
          led_timer_on_set[i] = 1;
        }
        else
          led_timer_on_set[i] = 0;
    }
    else
      led_timer_on_set[i] = 0;

    if ( led_timer_on_set[i] )
    {
      led_status[i] = 1;
      led_set[i] = 0;
    }
    else
    {
      led_status[i] = led_set[i];
    }

    if ( led_status_buf[i] != led_status[i] )
      update_blynk_status[i] = 1;  
  }
    // HARDWARE CONTROL
  

}

// #########################################################################################################
void blynk_update()
{
  if ( update_blynk_status[0] )
  {
      update_blynk_status[0] = 0;
      Blynk.virtualWrite(V9, led_status[0]);
  }  
  if(led_status[0]==0){
    analogWrite(Light, LOW);    
    Blynk.virtualWrite(V48, "OFF");
  }
  else if(led_status[0]==1){
    int i;
    uint16_t value=0;
    BH1750_Init(BH1750address);
    delay(200);
  
    if(2==BH1750_Read(BH1750address)){
    value=((buff[0]<<8)|buff[1])/1.2;
    Serial.print("Intensity in LUX: ");
    Serial.print(value);
    Serial.println("   ");
    }
    delay(150);
     
    float uMol = (value)*(0.089)*(55.38); //conversion factor

    Serial.print("Intensity = ");
    Serial.print(uMol); Serial.println(" uMol");
    
    float ilawValue = (uMol/55.38);
    Serial.print("ilawValue: ");
    Serial.println(ilawValue);
    Blynk.virtualWrite(V8, uMol);
    Blynk.virtualWrite(V48, "ON");

    int liwanag = map(OPTIMUM_LIGHT_INTENSITY, 0, 364, 0, 255);
    analogWrite(Light, liwanag);
    Serial.print("liwanag: ");
    Serial.println(liwanag);
    Serial.println(" \n");
   
   }
}

int BH1750_Read(int address) {
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) 
  {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();  
  return i;
}
 
void BH1750_Init(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x10);
  Wire.endTransmission();
}


void setup(){
  // Debug console
  Serial.begin(115200);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  //Blynk.begin(auth, wifi, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, wifi, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);
  dht.begin();
  timer.setInterval(100L, myTimerEvent);
  timer.setInterval(20L, sendSensor);
  timer.setInterval(20L, water_level);
  timer.setInterval(20L, soil_subsystem);
  timer.setInterval(20L, checkTime);


//soil subsystem
  pinMode(WATER_PUMP1, OUTPUT);
  pinMode(Pin1, INPUT);
  digitalWrite(WATER_PUMP1, HIGH);


//system water level
  pinMode(trigPin2, OUTPUT); // Sets the trigPin2 as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin2 as an Input
  digitalWrite(trigPin2, HIGH);


//light subsystem
  pinMode (Light, OUTPUT);
  pinMode(A3, OUTPUT);
  Wire.begin();
  Serial.println(SDA);
  Serial.println(SCL);

//climate susbsystem
  pinMode(ATOMIZER, OUTPUT);
  pinMode(HUMID_FAN, OUTPUT);
  pinMode(trigPin1, OUTPUT); // Sets the trigPin2 as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin2 as an Input
  digitalWrite(trigPin1, HIGH);
}

void loop(){
  Blynk.run();
  timer.run();
  led_mng();
  blynk_update();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

//Drop-Down Menu
BLYNK_WRITE(V13) {
  int index = param.asInt();
  switch (index)
  {
    case 1: //Lettuce
      opt_temp = 21;  
      opt_humid = 75;  
      opt_smoist = 25;  
      opt_lux = 24;  
      opt_water = 25;
      opt_photo = 18;  
      vegetable_choice = "Lettuce";
      Blynk.virtualWrite(V25, opt_temp);  
      Blynk.virtualWrite(V26, opt_humid);  
      Blynk.virtualWrite(V27, opt_smoist);  
      Blynk.virtualWrite(V28, opt_lux);  
      Blynk.virtualWrite(V30, opt_water);  
      Blynk.virtualWrite(V33, opt_photo);  
      break;
    case 2: //Pechay
      opt_temp = 20;  
      opt_humid = 75;  
      opt_smoist = 25;  
      opt_lux = 28;   
      opt_water = 25; 
      opt_photo = 16;
      vegetable_choice = "Pechay"; 
      Blynk.virtualWrite(V25, opt_temp);  
      Blynk.virtualWrite(V26, opt_humid);  
      Blynk.virtualWrite(V27, opt_smoist);  
      Blynk.virtualWrite(V28, opt_lux);  
      Blynk.virtualWrite(V30, opt_water);  
      Blynk.virtualWrite(V33, opt_photo);  
    case 3: //Mustard Greens
      opt_temp = 21;  
      opt_humid = 75;  
      opt_smoist = 25;  
      opt_lux = 25;   
      opt_water = 25; 
      opt_photo = 16; 
      vegetable_choice = "Mustard Greens"; 
      Blynk.virtualWrite(V25, opt_temp);  
      Blynk.virtualWrite(V26, opt_humid);  
      Blynk.virtualWrite(V27, opt_smoist);  
      Blynk.virtualWrite(V28, opt_lux);  
      Blynk.virtualWrite(V30, opt_water);  
      Blynk.virtualWrite(V33, opt_photo);  
      break;
    case 4: //Custom
      opt_temp = 15;  
      opt_humid = 75;  
      opt_smoist = 25;  
      opt_lux = 0;   
      opt_water = 25;  
      opt_photo = 12;
      vegetable_choice = "Custom"; 
      Blynk.virtualWrite(V25, opt_temp);  
      Blynk.virtualWrite(V26, opt_humid);  
      Blynk.virtualWrite(V27, opt_smoist);  
      Blynk.virtualWrite(V28, opt_lux);  
      Blynk.virtualWrite(V30, opt_water);  
      Blynk.virtualWrite(V33, opt_photo);  
      break;
  }
}

BLYNK_WRITE(V25) { //Optimum Temp Numerical Input
  int index = param.asInt();
  opt_temp = index;
}

BLYNK_WRITE(V26) { //Optimum Humid Numerical Input
  int index = param.asInt();
  opt_humid = index;
}

BLYNK_WRITE(V27) { //Optimum Soil Moisture Numerical Input
  int index = param.asInt();
  opt_smoist = index;
}

BLYNK_WRITE(V28) { //Optimum Light Intensity Numerical Input
  int index = param.asInt();
  opt_lux = index;
}

BLYNK_WRITE(V30) { //Optimum Water Level Numerical Input
  int index = param.asInt();
  opt_water = index;
}

BLYNK_WRITE(V33) { //Optimum Photoperiod Numerical Input
  int index = param.asInt();
  opt_photo = index;
}

//Confirm or Deactivate Button
BLYNK_WRITE(V29) {
  conf_deact = param.asInt();
  switch (conf_deact){
    case 0: //NOT CONFIRMED
      OPTIMUM_TEMPERATURE = DEFAULT_ZERO;
      OPTIMUM_HUMIDITY = DEFAULT_ZERO;
      OPTIMUM_SOIL_MOISTURE_LEVEL = DEFAULT_ZERO;
      OPTIMUM_LIGHT_INTENSITY = DEFAULT_ZERO;
      OPTIMUM_WATER_LEVEL = DEFAULT_ZERO;
      OPTIMUM_PHOTOPERIOD = DEFAULT_ZERO;
      Serial.print("Optimum Temp: ");
      Serial.println(OPTIMUM_TEMPERATURE);
      Serial.print("Optimum Humidity: ");
      Serial.println(OPTIMUM_HUMIDITY);
      Serial.print("Optimum Soil Moisture: ");
      Serial.println(OPTIMUM_SOIL_MOISTURE_LEVEL);
      Serial.print("Optimum Light Intensity: ");
      Serial.println(OPTIMUM_LIGHT_INTENSITY);
      Serial.print("Optimum Water Level: ");
      Serial.println(OPTIMUM_WATER_LEVEL);
      Blynk.virtualWrite(V36, OPTIMUM_TEMPERATURE);
      Blynk.virtualWrite(V37, OPTIMUM_HUMIDITY);
      Blynk.virtualWrite(V38, OPTIMUM_SOIL_MOISTURE_LEVEL);
      Blynk.virtualWrite(V39, OPTIMUM_WATER_LEVEL);
      Blynk.virtualWrite(V40, OPTIMUM_LIGHT_INTENSITY);
      Blynk.virtualWrite(V43, OPTIMUM_PHOTOPERIOD);
      Blynk.virtualWrite(V44, DEFAULT_LABEL);
      Blynk.virtualWrite(V25, DEFAULT_ZERO);
      Blynk.virtualWrite(V26, DEFAULT_ZERO);
      Blynk.virtualWrite(V27, DEFAULT_ZERO);
      Blynk.virtualWrite(V28, DEFAULT_ZERO);
      Blynk.virtualWrite(V30, DEFAULT_ZERO);
      Blynk.virtualWrite(V33, DEFAULT_ZERO);
      break;
    case 1: //CONFIRMED
      OPTIMUM_TEMPERATURE = opt_temp;
      OPTIMUM_HUMIDITY = opt_humid;
      OPTIMUM_SOIL_MOISTURE_LEVEL = opt_smoist;
      OPTIMUM_LIGHT_INTENSITY = opt_lux;
      OPTIMUM_WATER_LEVEL = opt_water;
      OPTIMUM_PHOTOPERIOD = opt_photo;
      Serial.print("Optimum Temp: ");
      Serial.println(OPTIMUM_TEMPERATURE);
      Serial.print("Optimum Humidity: ");
      Serial.println(OPTIMUM_HUMIDITY);
      Serial.print("Optimum Soil Moisture: ");
      Serial.println(OPTIMUM_SOIL_MOISTURE_LEVEL);
      Serial.print("Optimum Light Intensity: ");
      Serial.println(OPTIMUM_LIGHT_INTENSITY);
      Serial.print("Optimum Water Level: ");
      Serial.println(OPTIMUM_WATER_LEVEL);
      Blynk.virtualWrite(V36, OPTIMUM_TEMPERATURE);
      Blynk.virtualWrite(V37, OPTIMUM_HUMIDITY);
      Blynk.virtualWrite(V38, OPTIMUM_SOIL_MOISTURE_LEVEL);
      Blynk.virtualWrite(V39, OPTIMUM_WATER_LEVEL);
      Blynk.virtualWrite(V40, OPTIMUM_LIGHT_INTENSITY);
      Blynk.virtualWrite(V43, OPTIMUM_PHOTOPERIOD);
      Blynk.virtualWrite(V44, vegetable_choice);
      break;
 }
}
