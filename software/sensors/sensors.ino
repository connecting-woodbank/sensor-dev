// Sensors.ino

/* Sensor node fimrware August 2016
    version v1.0 - All sensors
    Andy Stevenson-Jones

    Arduino reads sensors and returns formatted data through the COM port
    Format: "Soil Moisture, Light (RGB), Soil Temperature, Air Temperature, Air Relative Humidity, IR Level"
*/


/* Pin allocations for Arduino
 *  Analog
  A0 - SoilMoisturePin: soil moisture analog output
  A4 - SDA (for I2C connecting RGB sensor)
  A5 - SCL (for I2C connecting RGB sensor)

 *  Digital
  D4 - SoilVccPin: power supply for soil moisture sensor
  D5 - DHT11_PIN: Air temperature/humidity digital output
  D6 - ONE_WIRE_BUS: Soil temperature digital output
*/

/*
 * VARIABLES
 * The values to send are:
 * SoilMoisture: unsinged int (0-1023)
 * red: signed int
 * green: unsigned int
 * blue: unsigned int
 * SoilTemp: signed char
 * AirTemp: signed char
 * AirHumidity: unsigned char
 */

 /*
  * MESSAGE STRUCTURE
      msg[0] = SoilMoisture UB
      msg[1] = SoilMoisture LB;
      msg[2] = red UB
      msg[3] = red LB
      msg[4] = green UB
      msg[5] = green LB
      msg[6] = blue UB
      msg[7] = blue LB
      msg[8] = SoilTemp
      msg[9] = AirTemp 
      msg[10] = AirHumidity
      msg[11] = photoIR UB
      msg[12] = photoIR LB
  */




// Libraries
#include <Wire.h>
#include <SparkFunISL29125.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// GENERAL DEFITIONS
// is it an arduino mini? - used to set clock rates
#define BAUD_RATE 9600

// Define pins for sensors
// Digital
#define DHT11_PIN 5   // DHT digital pin
#define ONE_WIRE_BUS 6  // Dallas temperature sensor digital pin

#define SoilMoisturePin  0  // analog read from soil moisture sensor, analog
#define SoilVccPin  4  // digital pin used as Vcc.  Sensor is connected from here to GND to reduce degredation of sensor
#define IRPin  1  // analog pin from IR photosensor
#define IRVccPin  3 // Vcc to IR photosensor


// DEFINE OBJECTS

// DHT11, air temperature/humidity
dht DHT;  // declare sensor object

// Dallas temperature sensor (soil)
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress soilThermometer; // arrays to hold device address

// ISL29125: RGB photosensor
SFE_ISL29125 RGB_sensor; // Declare sensor object


// begin setup
void setup() {  
  // general setup
  Serial.begin(BAUD_RATE);
  Serial.println("Serial Comms setup.");

  // ISL29125 RGB PHOTOSENSOR

  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Light sensor Initialization Successful\n\r");
  }


  // DALLAS T/RH SENSOR
  Serial.println("Initiliase Dallas T sensor");
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

  // Search for devices on the bus and assign based on an index.  Only one sensor shouldbe found here but more may be used.
  if (!sensors.getAddress(soilThermometer, 0)) Serial.println("Unable to find address for soil temperature sensor");

  // show the addresses found on the bus
  Serial.print("Soil temperature sensor address: ");
  printAddress(soilThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(soilThermometer, 9);
  Serial.print("Soil temperature resolution: ");
  Serial.print(sensors.getResolution(soilThermometer), DEC);
  Serial.println();


  // soil moisture
  pinMode(SoilVccPin, OUTPUT);  // Vcc pin for soil moisture sensor
  digitalWrite(SoilVccPin, LOW);   // disable sensor

  // IR phototransistor
  pinMode(IRVccPin, OUTPUT);  // set to out
  digitalWrite(IRVccPin, LOW);  // make sure it's off


  // COMPLETE SETUP
  Serial.println("Node setup complete.");
  Serial.println("--------------------------------------------");
  Serial.println();
  Serial.println("Soil Moisture, Light (RGB), Soil Temperature, Air Temperature, Air Relative Humidity, IR Level, DHT checksum error");
  delay(2000);
}

// BEGIN LOOP
void loop() {

  // SOIL MOISTURE
  digitalWrite(SoilVccPin, HIGH);   // enable sensor
  delay(50);  // allow power to settle
  unsigned int SoilMoisture = 1023 - (analogRead(SoilMoisturePin)); // read analog voltage then negate so that open circuit -> 0, short circtuit -> 1023
  digitalWrite(SoilVccPin, LOW);   // disable sensor


  // RGB PHOTOSENSOR
  // Read sensor values (16 bit integers)
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();


  // SOIL TEMPPERATURE
  sensors.requestTemperatures(); // Send the command to get temperatures
  signed char SoilTemp = sensors.getTempC(soilThermometer);


  // DHT AIR T/RH SENSOR
  int chk = DHT.read11(DHT11_PIN);
  int dht_checksum_error=0; // variable to hold error codes
  switch (chk)
  {
    case DHTLIB_OK:
      //               Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      //Serial.print("Checksum error,\t");
      dht_checksum_error |= 0x01;
      break;
    case DHTLIB_ERROR_TIMEOUT:
      //Serial.print("Time out error,\t");
      dht_checksum_error |= 0x02;
      break;
    case DHTLIB_ERROR_CONNECT:
      //Serial.print("Connect error,\t");
      dht_checksum_error |= 0x04;
      break;
    case DHTLIB_ERROR_ACK_L:
      //Serial.print("Ack Low error,\t");
      dht_checksum_error |= 0x08;
      break;
    case DHTLIB_ERROR_ACK_H:
      //Serial.print("Ack High error,\t");
      dht_checksum_error |= 0x10;
      break;
    default:
      //Serial.print("Unknown error,\t");
      dht_checksum_error |= 0x80;
      break;
  }
  // display data
  signed char AirTemp = DHT.temperature;
  unsigned char AirHumidity = DHT.humidity;


  //IR PHOTOSENSOR
  digitalWrite(IRVccPin, HIGH);   // enable sensor
  int photoIR = analogRead(IRPin); // read IR level
  digitalWrite(IRVccPin, LOW);   // disable sensor


  // construct message
  unsigned char msg[13];
  msg[0] = (SoilMoisture >> 8) & 0xFF;
  msg[1] = SoilMoisture & 0xFF;
  msg[2] = (red >> 8) & 0xFF;
  msg[3] = red & 0xFF;
  msg[4] = (green >> 8) & 0xFF;
  msg[5] = green & 0xFF;
  msg[6] = (blue >> 8) & 0xFF;
  msg[7] = blue & 0xFF;
  msg[8] = SoilTemp & 0xFF;
  msg[9] = AirTemp & 0xFF;
  msg[10] = AirHumidity & 0xFF;
  msg[11] = (photoIR >> 8) & 0xFF;
  msg[12] = photoIR & 0xFF;

  // display msg
  /*
  for (int i=0; i<sizeof(msg); i++)
  {
    Serial.print(msg[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
*/

  // DISPLAY data
  Serial.print(SoilMoisture, DEC); // soil moisture level
  Serial.print(", ");
  Serial.print(red, DEC); // RGB: RED
  Serial.print(", ");
  Serial.print(green, DEC); // RGB: GREEN
  Serial.print(", ");
  Serial.print(blue, DEC); // RGB: BLUE
  Serial.print(", ");
  Serial.print(SoilTemp, DEC);  // soil temperature
  Serial.print(", ");
  Serial.print(AirTemp, DEC); // Air temperature
  Serial.print(", ");
  Serial.print(AirHumidity, DEC); // Air Humidity
  Serial.print(", ");
  Serial.print(photoIR);  // IR level
  Serial.print(", ");
  Serial.print(dht_checksum_error);  // DHT checksum error


  // finish
  Serial.println();
  delay(2000);


}

// FUNCTION DEFINTIONS

// soil temperature function to print the temperature for a device
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

// function to print a device address of soil temperature sensor
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
