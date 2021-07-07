#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int Sensor_Pin = A0;
int Sensitivity = 66;
float Vpp = 0; // peak-peak voltage 
float Vrms = 0; // rms voltage
float Irms = 0; // rms current
float Supply_Voltage = 230.0;           // reading from DMM
float Vcc = 5.0;         // ADC reference voltage // voltage at 5V pin 
float power = 0;         // power in watt              
float Wh =0 ;             // Energy in kWh
float Watt=0;
unsigned long last_time =0;
unsigned long current_time =0;
unsigned long interval = 100;
unsigned int calibration = 100;  // V2 slider calibrates this
unsigned int pF = 85;           // Power Factor default 95
float bill_amount = 0;   // 30 day cost as present energy usage incl approx PF 
unsigned int energyTariff = 8.0; 

float getVPP()
{
  float result; 
  int readValue;                
  int maxValue = 0;             
  int minValue = 1024;          
  uint32_t start_time = millis();
  while((millis()-start_time) < 1000) //read every 0.95 Sec
  {
     readValue = analogRead(Sensor_Pin);    
     if (readValue > maxValue) 
     {         
         maxValue = readValue; 
     }
     if (readValue < minValue) 
     {          
         minValue = readValue;
     }
  } 
   result = ((maxValue - minValue) * Vcc) / 1024.0;  
   return result;
}

void getACS712() {  // for AC
  Vpp = getVPP();
  Vrms = (Vpp/2.0) *0.707; 
  //Vrms = Vrms - (calibration / 10000.0);     // calibtrate to zero with slider
  Irms = (Vrms * 1000)/Sensitivity ;
  //Irms=Irms+4.6;
  if((Irms > -0.015) && (Irms < 0.008)){  // remove low end chatter
    Irms = 0.0;
  }
    //Irms=Irms+3.6;
  power= (Supply_Voltage * Irms) * (pF / 100.0); 
  //power=power+881;
  last_time = current_time;
  current_time = millis();    
  Wh = Wh+  power *(( current_time -last_time) /3600000.0) ; // calculating energy in Watt-Hour
  Watt=Wh/1000;
  bill_amount = power * (energyTariff);

  Serial.print("Supply Voltage:  "); 
  Serial.print(String(Supply_Voltage, 3));
  Serial.println(" V");
  lcd.setCursor(0,0);
  lcd.print("Voltage =     ");
  lcd.setCursor(10,0);
  lcd.print(String(Supply_Voltage));
  lcd.setCursor(15,0);
  lcd.print("V");

  delay(1000);

  Serial.print("Current:  "); 
  Serial.print(String(Irms, 3));
  Serial.println(" A");
  lcd.setCursor(0,0);
  lcd.print("Current =     ");
  lcd.setCursor(10,0);
  lcd.print(String(Irms));
  lcd.setCursor(15,0);
  lcd.print("A");
  
  delay(1000);

  Serial.print("Power: ");   
  Serial.print(String(power, 3)); 
  Serial.println(" W");
  lcd.setCursor(0,0);
  lcd.print("Power =     ");
  lcd.setCursor(10,0);
  lcd.print(String(power));
  lcd.setCursor(15,0);
  lcd.print("W");
  
  delay(1000); 
  
  Serial.print("Energy: ");   
  Serial.print(String(Wh, 3)); 
  Serial.println(" kW");
  lcd.setCursor(0,0);
  lcd.print("Energy =     ");
  lcd.setCursor(10,0);
  lcd.print(String(Wh));
  lcd.setCursor(14,0);
  lcd.print("kW");
  
  delay(1000);

  Serial.println();
}


void setup() 
{
  Serial.begin(9600);
  pinMode(Sensor_Pin, INPUT);           
  lcd.begin(16, 2);
  lcd.print("   Electrack ");
  delay(1000);
}
 
void loop()
{
  getACS712();
  delay(1000);
}
