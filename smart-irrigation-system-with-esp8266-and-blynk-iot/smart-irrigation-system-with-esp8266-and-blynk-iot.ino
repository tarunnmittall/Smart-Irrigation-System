#define BLYNK_TEMPLATE_ID "TMPL3yNqBomup"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System"
#define BLYNK_AUTH_TOKEN "C7KUTDjYDpP3I4nzwsPj33HdtZkeApLA"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h> 

#define SOIL_MOISTURE_PIN A0   // Analog pin for soil moisture sensor
#define THRESHOLD_MOISTURE 10 // Adjust this value based on your soil moisture sensor readings
#define PUMP_PIN D1  // Digital pin for controlling the pump
#define PUMP_SWITCH V6  // Virtual pin for controlling the pump manually
#include <DHT.h>
char auth[] = BLYNK_AUTH_TOKEN;  // Replace with your Blynk auth token
char ssid[] = "garima";   // Replace with your WiFi credentials
char pass[] = garimakhatri";

BlynkTimer timer;

#define DHTPIN 4 //Connect Out pin to D2 in NODE MCU
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

bool isPumpOn = false;  // Variable to track pump status

void sendSensorData()
{
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  Serial.print("Soil Moisture ");
  Serial.println(soilMoisture);
  // Map the analog sensor values to a percentage (0-100)
  int soilMoisturePercentage = map(soilMoisture, 400, 1023, 100, 0);
  Serial.print("Soil Moisture Percentage ");
  Serial.println(soilMoisturePercentage);
  // Send soil moisture data to Blynk
  Blynk.virtualWrite(V5, soilMoisturePercentage);

  // Check if the pump should be on based on manual switch or soil moisture level
  if (isPumpOn || soilMoisturePercentage < THRESHOLD_MOISTURE)
  {
    // Turn on the pump
    digitalWrite(PUMP_PIN, HIGH);
    // Check if the pump is turned on automatically (not manually)
    if (!isPumpOn) {
      // Send alert notification to Blynk app if the pump is turned on automatically
      Blynk.logEvent("moisture_alert","Soil moisture is below the threshold!");
      Serial.println("Soil moisture is below the threshold!");
    }
  }
  else
  {
    // Turn off the pump only if it was not turned on manually
    if (!isPumpOn) {
      digitalWrite(PUMP_PIN, LOW);
    }
  }
  
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  /*int soilmoisturevalue = analogRead(A0);
   soilmoisturevalue = map(soilmoisturevalue, 0, 1023, 0, 100);*/
   //int value = analogRead(A0);
  //value = map(value,400,1023,100,0);
  //float h = dht.readHumidity();
  //float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
}
BLYNK_WRITE(PUMP_SWITCH) 
{
  isPumpOn = param.asInt();
  if (isPumpOn) {
    Serial.println("Pump manually turned ON");
  } else {
    Serial.println("Pump manually turned OFF");
  }
    // You can send any value at any time.
   // Please don't send more that 10 values per second.
    //Blynk.virtualWrite(V0, value);
    
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
    //Serial.print("Soil Moisture : ");
    //Serial.print(value);
  Serial.print("Temperature : ");
  Serial.print(t);
  Serial.print("    Humidity : ");
  Serial.println(h);
}

void setup()
{
  Serial.begin(9600);
  //Serial.begin(115200);
  pinMode(PUMP_PIN, OUTPUT); // Set pump pin as an output
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(3000L, sendSensorData); // Set the interval for checking soil moisture (every 3 seconds)
 // timer.setInterval(100L, sendSensor);
  // Setup switch widget
  Blynk.virtualWrite(PUMP_SWITCH, isPumpOn);
  Blynk.syncVirtual(PUMP_SWITCH);

}

void loop()
{
  Blynk.run();
  timer.run();
}
