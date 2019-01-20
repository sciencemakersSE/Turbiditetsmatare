/* Ett program som mäter turbiditet med hjälp av en vit diod och två sensorer
   av typen TSL 2561.

Senast uppdaterad: 17 augusti 2018 av Pernilla Wegén
   
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

float lum_90=0;
float lum_180=0;
float kvot;
float lnkvot;
int serialInput = 0; // variabel som håller reda på senaste tangentinmatningen


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
  configureSensor();
  
  pinMode(13,OUTPUT);
  Serial.println(" Starting..");
  if (Serial.available() >= 0)
  {                             // returnerar -1 om ingen tangentinmatning är tillgänglig
    Serial.println("Waiting for input. Press m for measuring. ");
  }
  //Serial.print("90 grader (LUX)");
  //Serial.print("180 grader (LUX)");
  //Serial.println("turbiditet");
}



//Här har vi själva programmet!

void loop(void) 
{  
  if (Serial.available() > 0)
  {                           // om tangentinmatning är tillgänglig, läs in vad..
    serialInput = Serial.read();
    switch (serialInput)
    { 
      case 'm':
        lum_90 = 0;     //nollställer variablerna
        lum_180 = 0;  
        Serial.println("Measuring...");
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
  /* Visa resultaten (ljuset mäts i lux) */
  
   
   //Serial.print(lum_90); Serial.print("         ");
   //Serial.print(lum_180); Serial.print("        ");
  
  //här ska vi göra en beräkning
  
    kvot = (lum_90/lum_180);
 
    lnkvot = -log(kvot);
    Serial.print("turbiditet:   ");
    Serial.println(lnkvot);
  
  delay(250);
  digitalWrite(13,LOW);
 
}}
}
