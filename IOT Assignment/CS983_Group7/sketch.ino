#include <Servo.h>
#include <dht.h>
#include <Stepper.h>
#include <LiquidCrystal.h>

#define PIN_TRIG 12
#define PIN_ECHO 11
#define DHT22_PIN 14
#define LED_PIN 52
#define SERVO_PIN 10

// LCD for display
LiquidCrystal lcd(32, 34, 36, 38, 40, 42);

// DHT for temp & Humidity sensor
dht DHT;

// Stepper motor for Fan.
Stepper myStepper(2000, 16, 17, 18, 19);

// Servo for Door control
Servo myservo;
int servo_position = 90;

// Global variable for display
bool g_doorOpen = false;
bool g_lightOn = false;
int g_fanSpeed = 0;
float g_temp = 0;
float g_humid = 0;

void setup() 
{
  // Attch the servo pin to the servo object
  myservo.attach(SERVO_PIN);  

  // Set input/output pins for Ultrasonic Distance Sensor
  pinMode(PIN_TRIG, OUTPUT); 
  pinMode(PIN_ECHO, INPUT); 

  pinMode(LED_PIN, OUTPUT);

  myStepper.setSpeed(200);

  lcd.begin(20, 4);
  lcd.setCursor(6,0); 
  lcd.print("GROUP 7"); 
  lcd.setCursor(3,1) ; //sets cursor to second line first row
  lcd.print("IOT Assignment");
  delay(1000);
}

/// Shows parameters on LCD.
void refershDisplay()
{
    lcd.setCursor(0,1) ;
    lcd.print("T:");
    lcd.print(g_temp);
    lcd.print("(C) H:");
    lcd.print(g_humid);
    lcd.print("% ");
    lcd.setCursor(0,2); 
    lcd.print("Fan:");
    lcd.print(g_fanSpeed);
    lcd.print(" Lights:");
    lcd.print(g_lightOn?"ON ":"OFF");
    lcd.setCursor(0,3); 
    lcd.print("Door:");
    lcd.print(g_doorOpen?"OPEN ":"CLOSE");

}

//===================================================================
// Door Movement

// Detects is someone is at the door(<200cm distance) using Ultrasonic Distance Sensor
// return true is distance from sensor < 200 cm.
//else return false.
bool IsSomeoneAtDoor()
{
  long duration = 0;
  int distance = 0;

  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Reads the sound wave travel time in microseconds
  duration = pulseIn(PIN_ECHO, HIGH);
  // Calculating the distance
  distance = (duration * 0.034 / 2);

  return (distance < 200);
}


// If someone is at door then open the door
void controlDoor(){
 
  if(IsSomeoneAtDoor())
  { 
    // Open the door
    g_doorOpen = true;
    refershDisplay();

    if(servo_position > 0)
    {
      servo_position -= 5;
      myservo.write(servo_position);
    }
  }
  else
  {
    // Close the door
    g_doorOpen = false;
    if(servo_position < 90){
      servo_position += 5;
      myservo.write(servo_position);
    }
  }
}

// - Door Movement
//===================================================================

//===================================================================
// Light On/Off

// Detect illumintion(lux) using the light sensor.
//if lux < 400 then turn on the lights(LED)
void controlLights()
{
  const float GAMMA = 0.7;
  const float RL10 = 50;

  // Convert the analog value into lux value:
  int analogValue = analogRead(A0);
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

  if(lux<400)
  {
    g_lightOn = true;
    digitalWrite(LED_PIN, HIGH);   // turn on the lights        
  }
  else
  {
    g_lightOn = false;
    digitalWrite(LED_PIN, LOW);    // turn off the lights 
  }
  refershDisplay();
}

// - Light On/Off
//===================================================================

//===================================================================
// Fan Speed control
/*
Adjust fan speed according to following parameters.
Temperature 	Humidity 	Fan speed
25 and below 					0
25-29 			40-60 			2
25-29 			61-80 			3
25-29 			81-100 			4
30-34 			40-60 			3			
30-34 			61-80 			4
30-34 			81-100 			5
35-39 			40-60 			4
35-39 			61-100 			5
40 and above 	40-100 			5

Note we have added following fan speed for 
humidity less than 40  :
Temperature 	Fan speed
25-29 			    1
30-34           2
35-39           3
40 & above      4
*/
void controlFan()
{
  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();
  
  g_temp = DHT.temperature;
  g_humid = DHT.humidity;
  
  if(g_temp<=25)
  {
    myStepper.step(0);
    g_fanSpeed = 0;
  }
  else if(g_temp > 25 and g_temp <= 29)
  {
    if(g_humid >= 40 and g_humid <= 60){
      myStepper.step(-10);
      g_fanSpeed = 2;
    }
    else if(g_humid > 60 and g_humid <= 80)
    {
      myStepper.step(-50);
      g_fanSpeed = 3;
    }
    else if(g_humid > 80 and g_humid <= 100){
      myStepper.step(-100);
      g_fanSpeed = 4;
    }
    else{
      myStepper.step(-5);
      g_fanSpeed = 1;
    }
  }
  else if(g_temp > 29 and g_temp <= 34)
  {
    if(g_humid >= 40 and g_humid <= 60)
    {
      myStepper.step(-50);
      g_fanSpeed = 3;
    }
    else if(g_humid > 60 and g_humid <= 80)
    {
      myStepper.step(-100);
      g_fanSpeed = 4;
    }
    else if(g_humid > 80 and g_humid <= 100)
    {
      myStepper.step(-150);
      g_fanSpeed = 5;
    }
    else{
      myStepper.step(-10);
      g_fanSpeed = 2;
    }
  }
  else if(g_temp > 34 and g_temp <= 39)
  {
    if(g_humid >= 40 and g_humid <= 60)
    {
      myStepper.step(-100);
      g_fanSpeed = 4;
    }
    else if(g_humid > 60 and g_humid <= 100)
    {
      myStepper.step(-150);
      g_fanSpeed = 5;
    }
    else
    {
      myStepper.step(-50);
      g_fanSpeed = 3;
    }
  }
  else if(g_temp > 39)
  {
    if(g_humid >= 40 and g_humid <= 100)
    {
      myStepper.step(-150);
      g_fanSpeed = 5;
    }
    else
    {
      myStepper.step(-100);
      g_fanSpeed = 4;
    }
  }
}

// - Fan speed
//===================================================================


void loop() {
  controlDoor();
  controlLights();
  controlFan();
  refershDisplay();
}

