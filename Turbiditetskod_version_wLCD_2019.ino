/* Ett program som mäter turbiditet med hjälp av en vit diod och två sensorer
   av typen TSL 2561.

Senast uppdaterad: 7 maj 2019 av Pernilla Wegén
   
   Du behöver ladda ner Adafruits sensorbibliotek (Adafruit_Sensor) och inkludera
   det i ditt bibliotek.
   
   Nedan följer lite kopplingsinstruktioner:
   ===========
   Koppla SCL till analog 5
   Koppla SDA till analog 4
   Koppla 3Vo till 3.3V DC
   Koppla GROUND till common ground

   I2C Address
   ===========
   
*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float lum_90=0;
float lum_180=0;
float kvot;
float turbidity;
int serialInput = 0; // variabel som håller reda på senaste tangentinmatningen
int buttonState = 0;  // håller reda på om knappen är intryckt eller ej

Adafruit_TSL2561_Unified tsl1 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 12345);
Adafruit_TSL2561_Unified tsl2 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/**************************************************************************/
/*
    Konfigurerar förstärkning och mättid för din TSL2561. Här kan man göra ändringar om man vill.
*/
/**************************************************************************/
void configureSensor(void)
{
  
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  tsl1.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl2.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  // tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */  
}

/**************************************************************************/
/*
    Uppstartskommandon
*/
/**************************************************************************/
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Press button for measuring. ");
  
  lcd.begin(16, 2);
  lcd.print("Press button to");
  lcd.setCursor(0, 1);
  lcd.print("measure.");
  
  configureSensor();
  
  pinMode(13,OUTPUT);
  pinMode(8, INPUT);
}

//Här har vi själva programmet!

void loop(void) 
{  
  buttonState = digitalRead(8);
  if(buttonState == HIGH)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Measuring");

    Serial.println("Measuring...");
      
    lum_90 = 0;     //nollställer variablerna
    lum_180 = 0;  
        
    digitalWrite(13,HIGH);
    delay(3000);
  
    for (int i=0; i<5; i++)
    {
      sensors_event_t event1;
      sensors_event_t event2;
      tsl1.getEvent(&event1);
      tsl2.getEvent(&event2); 
      lum_90 = lum_90 + event1.light;
      lum_180 = lum_180 + event2.light;
    }
    
    lum_90 = lum_90 / 5;
    lum_180 = lum_180 / 5;
    
    kvot = (lum_90/lum_180);
    turbidity = -log(kvot);
    
    Serial.print("Turbidity:   ");
    Serial.println(turbidity);
    
    lcd.setCursor(0, 0);
    lcd.print("Turbidity:");
    lcd.setCursor(0, 1);
    lcd.print(turbidity);
  
    delay(250);
    digitalWrite(13,LOW);

  }
}
