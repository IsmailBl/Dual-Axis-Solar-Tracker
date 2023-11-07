//Calling for Libraries
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Servo.h> // include Servo library 
// 180 horizontal MAX
Servo horizontal; // horizontal servo
int servoh = 180;   // 90;     // stand horizontal servo
int servohLimitHigh = 180;
int servohLimitLow = 65;
int dtime = 10;
// 65 degrees MAX
Servo vertical;   // vertical servo 
int servov = 45;    //   90;     // stand vertical servo

int servovLimitHigh = 80;
int servovLimitLow = 15;

// LDR pin connections

int ldrlt = A2; //LDR top left - BOTTOM LEFT    <--- BDG
int ldrrt = A1; //LDR top rigt - BOTTOM RIGHT 
int ldrld = A3; //LDR down left - TOP LEFT
int ldrrd = A0; //ldr down rigt - TOP RIGHT

const int rain_detector  = 3; 
const int voltage_sensor = A4; 
const int current_sensor = A5; 
int current_value;

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
 
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
 
// Float for Reference Voltage
float ref_voltage = 5.0;
 
// Integer for ADC value
int adc_value = 0;
bool rain = false;
const double scale_factor = 0.1; // 20A
double Vout = 0;
double Current = 0;
const double vRef = 5.00;
const double resConvert = 1024;
double resADC = vRef/resConvert;
double zeroPoint = vRef/2;
int solar_p =0; 
void setup()
{
  Serial.begin(9600);
// servo connections
// name.attacht(pin);
  horizontal.attach(9); 
  vertical.attach(10);
  horizontal.write(180);
  vertical.write(45);
  delay(3000);
  pinMode(rain_detector, INPUT); 
  pinMode(voltage_sensor, INPUT); 
  pinMode(current_sensor, INPUT); 
  lcd.init();                       
  lcd.backlight();
  lcd.home();
  lcd.setCursor(0, 0);                // place static text
  lcd.print("Rain= ");
  lcd.setCursor(0, 1);
  lcd.print("V= ");
 
  lcd.setCursor(6, 1);
  lcd.print("I= "); 
}

void loop() 
{
   rainDetector();
   current();
   voltage();
   delay(dtime);
   tracker();
   delay(dtime);
   solar_p = Current*in_voltage; 
   Serial.print(solar_p);

}

void current(){
   // Vout is read 1000 Times for precision
  for(int i = 0; i < 1000; i++) {
    Vout = (Vout + (resADC * analogRead(A0)));   
    delay(1);
  }
  
  // Get Vout in mv
  Vout = Vout /1000;
  
  // Convert Vout into Current using Scale Factor
  Current = (Vout - zeroPoint)/ scale_factor;   
    lcd.setCursor(9, 1);
  lcd.print(Current); 
}
void voltage(){
  adc_value = analogRead(voltage_sensor);
   
   // Determine voltage at ADC input
   adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
   
   // Calculate voltage at divider input
   in_voltage = adc_voltage / (R2/(R1+R2)); 
   
   // Print results to Serial Monitor to 2 decimal places
  
  lcd.setCursor(4, 1);
  lcd.print(in_voltage);
  
}
void rainDetector(){
  rain == digitalRead(rain_detector);
    if (rain == true ){
       lcd.setCursor(6, 0); 
       lcd.print("Detected");
    }
    else{
       lcd.setCursor(6, 0); 
       lcd.print("Nope");
    }
}
void tracker(){
  int lt = analogRead(ldrlt); // top left
  int rt = analogRead(ldrrt); // top right
  int ld = analogRead(ldrld); // down left
  int rd = analogRead(ldrrd); // down rigt
 
  int tol = 50;
  
  int avt = (lt + rt) / 2; // average value top
  int avd = (ld + rd) / 2; // average value down
  int avl = (lt + ld) / 2; // average value left
  int avr = (rt + rd) / 2; // average value right

  int dvert = avt - avd; // check the diffirence of up and down
  int dhoriz = avl - avr;// check the diffirence og left and rigt
  
  
 /* Serial.print(avt);
  Serial.print(" ");
  Serial.print(avd);
  Serial.print(" ");
  Serial.print(avl);
  Serial.print(" ");
  Serial.print(avr);
  Serial.print("   ");
  Serial.print(dtime);
  Serial.print("   ");
  Serial.print(tol);
  Serial.println(" ");*/
  
    
  if (-1*tol > dvert || dvert > tol) // check if the diffirence is in the tolerance else change vertical angle
  {
  if (avt > avd)
  {
    servov = ++servov;
     if (servov > servovLimitHigh) 
     { 
      servov = servovLimitHigh;
     }
  }
  else if (avt < avd)
  {
    servov= --servov;
    if (servov < servovLimitLow)
  {
    servov = servovLimitLow;
  }
  }
  vertical.write(servov);
  }
  
  if (-1*tol > dhoriz || dhoriz > tol) // check if the diffirence is in the tolerance else change horizontal angle
  {
  if (avl > avr)
  {
    servoh = --servoh;
    if (servoh < servohLimitLow)
    {
    servoh = servohLimitLow;
    }
  }
  else if (avl < avr)
  {
    servoh = ++servoh;
     if (servoh > servohLimitHigh)
     {
     servoh = servohLimitHigh;
     }
  }
  else if (avl = avr)
  {
    // nothing
  }
  horizontal.write(servoh);
  }
}
