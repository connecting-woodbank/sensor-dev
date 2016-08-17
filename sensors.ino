// Include
#include <Wire.h>
#include "SparkFunISL29125.h"
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>



// Air temperature/humidity: DHT11
dht DHT;  // define object
#define DHT11_PIN 5

// Soil temperature: DS
// Data wire is plugged into port 6 on the Arduino
#define ONE_WIRE_BUS 6
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress soilThermometer;

// Soil humidity
#define SoilMoisturePin  0  // pin A0
#define SoilVccPin  4  // digital pin used as Vcc.  Sensor is connected from here to GND to reduce degredation of sensor

// IR
#define IRPin  1  // A1
#define IRVccPin  3 // D3 - used to power soil moisture sensor  
  
// Light level: ISL29125
// Declare sensor object
SFE_ISL29125 RGB_sensor;


// Pin allocations    
//    	- A0: soil moisture analog output
// 		- A1: IR phototransistor analog output
//    	- A4: SDA (RGB sensor)
//    	- A5: SCL (RGB sensor)
//      - D4: Soil moisture Vcc
//      - D3: IR phototransistor Vcc 
//    	- D5: Air temperature/humidity digital output
// 		- D4: IR phototransistor power supply
// 		- D3: power supply for soil moisture sensor
//    	- D6: Soil temperature digital output


void setup(){
  // general setup
  Serial.begin(9600);
  // Light Sensor
  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Light sensor Initialization Successful\n\r");
  } 
  
  // temperature/humidity
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Assign address manually. The addresses below will beed to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  // Note that you will need to use your specific address here
  //soilThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

  // Method 1:
  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  if (!sensors.getAddress(soilThermometer, 0)) Serial.println("Unable to find address for soil temperature sensor"); 
  
  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them. It might be a good idea to 
  // check the CRC to make sure you didn't get garbage. The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to soilThermometer
  //if (!oneWire.search(soilThermometer)) Serial.println("Unable to find address for soilThermometer");

  // show the addresses we found on the bus
  Serial.print("Soil temperature sensor address: ");
  printAddress(soilThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(soilThermometer, 9);
 
  Serial.print("Soil temperature resolution: ");
  Serial.print(sensors.getResolution(soilThermometer), DEC); 
  Serial.println();

  // DHT11
  
 
  // soil moisture
  pinMode(SoilVccPin, OUTPUT);  // Vcc pin for soil moisture sensor
  digitalWrite(SoilVccPin, LOW);   // disable sensor
  
  // IR phototransistor
  pinMode(IRVccPin, OUTPUT);  // set to out
  digitalWrite(IRVccPin, LOW);  // make sure it's off
  
  // complete
  Serial.println("Node setup complete.");
  Serial.println("--------------------------------------------");
  Serial.println();
  Serial.println("Sol Moisture, Light (RGB), Soil Temperature, Air Relative Humidity, Air Temperature, IR Level");
  delay(5000);
}

void loop(){
  // Soil Moisture
  digitalWrite(SoilVccPin, HIGH);   // enable sensor
  delay(50);  // allow power to settle
  Serial.print(1023-(analogRead(SoilMoisturePin)));
  Serial.print(", ");
  digitalWrite(SoilVccPin, LOW);   // disable sensor
  
  // Light
    // Read sensor values (16 bit integers)
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();
  
  Serial.print(red,DEC);
  Serial.print(", ");
  Serial.print(green,DEC);
  Serial.print(", ");
  Serial.print(blue,DEC);
  Serial.print(", ");
  
  // soil temperature
    // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  // It responds almost immediately. Let's print out the data
  
  printTemperature(soilThermometer); // Use a simple function to print out the data

  // temperature and moisture
   // READ DATA
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
 //               Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(", ");
  Serial.print(DHT.temperature, 1);
  Serial.print(", ");
  
  //IR 
  digitalWrite(IRVccPin, HIGH);   // enable sensor
  Serial.print(analogRead(IRPin));
  digitalWrite(IRVccPin, LOW);   // disable sensor

  
  // finish
  Serial.println();
  delay(2000);
  
}

// soil temperature
// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(tempC);
  Serial.print(", ");
//  Serial.print(" Temp F: ");
//  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
