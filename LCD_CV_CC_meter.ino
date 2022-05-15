#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
LiquidCrystal_I2C lcd(0x27,16,2);  

#define relay 13
#define voltagePin A0
#define currentPin A1
#define tempPin A6
float AcsValueF=0.0;
float tempreature = 0;
float tempThreshold = 70;
int sampleCount = 150;

float voltage = 0;
float VR1 = 10000.0;
float VR2 = 2500.0;  

int Vo;
float R1 = 1000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

bool en = false;
bool en1 = false;
bool start = false;

void setup()
{
  pinMode(relay, OUTPUT);
  pinMode(voltagePin, INPUT);
  pinMode(currentPin, INPUT);
  pinMode(tempPin, INPUT);
  digitalWrite(relay, LOW);
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
  Timer1.initialize(500000);
  Timer1.attachInterrupt(Isr);
  lcd.setCursor(0,0);
  lcd.print("Initializing...");
  delay(100);
  if (analogRead (voltagePin) >= 0){
  start = true;
  }
  else{
    start = false;
  }


  lcd.clear();


}

void loop()

{

if (start == true)
{
  run();
}

else{
  lcd.setCursor(0,0);
  lcd.print("Set voltage to 0");
  lcd.setCursor(0,1);
  lcd.print("and restart");
}

  

}

void run(){

if (T >= tempThreshold || voltage >= 24){
  digitalWrite(relay, LOW);
  if (en == true){
lcd.clear();
  }
  
lcd.setCursor(5,0);
lcd.print("ERROR");
lcd.setCursor(0,1);
lcd.print("Temp: " + String(T) + "C");

en = false;
en1 = true;
}
else{
digitalWrite(relay, HIGH);
en = true;
  if (en1 == true){
    lcd.clear();
  }
  lcd.setCursor(0,0);
  lcd.print("VOLTAGE");
  lcd.setCursor(0,1);
  lcd.print(voltage);
  lcd.setCursor(5,1);
  lcd.print("V");
  lcd.setCursor(9,0);
  lcd.print("CURRENT");
  lcd.setCursor(9,1);
  lcd.print(AcsValueF);
  lcd.setCursor(15,1);
  lcd.print("A");
en1 = false;
}
}

void Isr(){
  voltageSens();
  currentSens();
  tempSens();

}

void voltageSens(){
  unsigned int x=0;
  float voltageValue=0.0, VoltageSamples=0.0,AvgVoltageSamples=0.0; 

  for (int x = 0; x < sampleCount; x++){ //Get samples
  voltageValue = analogRead(voltagePin);     //Read voltage divider values   
  VoltageSamples = VoltageSamples + voltageValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms

}
  AvgVoltageSamples = VoltageSamples/sampleCount;
  float VoltageMap = AvgVoltageSamples * (5.0/1023);
  voltage = VoltageMap / (VR2/(VR1+VR2));
}

void currentSens(){
  unsigned int x=0;
  float AcsValue=0.0, Samples=0.0,AvgAcs=0.0;

  for (int x = 0; x < sampleCount; x++){ //Get samples
  AcsValue = analogRead(currentPin);     //Read current sensor values   
  Samples = Samples + AcsValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
AvgAcs=Samples/sampleCount;//Taking Average of Samples
AcsValueF = (((AvgAcs * (5.0 / 1023.0))-3 )/0.066)*1;
}

void tempSens(){
  unsigned int x=0;
  float thermisterValue=0.0, TempSamples=0.0,Avgtemp=0.0;
  for (int x = 0; x < sampleCount; x++){ //Get samples
  thermisterValue = analogRead(tempPin);     //Read tempreature sensor values   
  TempSamples = TempSamples + thermisterValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
Avgtemp = TempSamples/sampleCount;


  Vo = Avgtemp;
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 373.15;

  //T = (T * 9.0)/ 5.0 + 32.0; 

}