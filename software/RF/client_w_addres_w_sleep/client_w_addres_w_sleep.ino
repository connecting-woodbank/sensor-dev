/*
 * Read sensors and transmit data over radio
 * 
 * Message format
 * Length: 20 bytes
 * format:
  msg[0] = (tx_count>>8) & 0xFF;
  msg[1] = tx_count & 0xFF;
  msg[2] = (red >> 8) & 0xFF;
  msg[3] = red & 0xFF;
  msg[4] = (green >> 8) & 0xFF;
  msg[5] = green & 0xFF;
  msg[6] = (blue >> 8) & 0xFF;
  msg[7] = blue & 0xFF;
  msg[8] = SoilMoisture & 0xFF;
  msg[9] = SoilTemp & 0xFF;
  msg[10] = AirTemp & 0xFF;
  msg[11] = AirHumidity & 0xFF;
  msg[12] = (dht_checksum_error>>8) & 0xFF;
  msg[13] = dht_checksum_error & 0xFF;

 Pin allocations for Arduino
 *  Analog
  A0 - SoilMoisturePin: soil moisture analog output
  A4 - SDA (for I2C connecting RGB sensor)
  A5 - SCL (for I2C connecting RGB sensor)

 *  Digital
  D4 - SoilVccPin: power supply for soil moisture sensor
  D5 - DHT11_PIN: Air temperature/humidity digital output
  D6 - ONE_WIRE_BUS: Soil temperature digital output

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

#include <LowPower.h>
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>
#include <Wire.h>
#include <dht.h>
#include <SparkFunISL29125.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// SENSORS
// DHT11
#define DHT11_PIN 5
dht DHT;
// ISL29125: RGB photosensor
SFE_ISL29125 RGB_sensor; // Declare sensor object
// Dalla temperature sensor
#define ONE_WIRE_BUS 6  // Dallas temperature sensor digital pin
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress soilThermometer; // arrays to hold device address
// Soil moisture sensor
#define SoilMoisturePin  0  // analog read from soil moisture sensor, analog
#define SoilVccPin  4  // digital pin used as Vcc.  Sensor is connected from here to GND to reduce degredation of sensor


//RF
#define CLIENT_ADDRESS 2 //self address
#define msg_length 20 // length of message
uint8_t SERVER_ADDRESS = 1; //address of the server node
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

// declare variables
signed char AirTemp=0;
unsigned char AirHumidity=0;
unsigned int red=0;
unsigned int blue=0;
unsigned int green=0;
signed char SoilTemp=0;
unsigned int SoilMoisture=0;
unsigned char msg[msg_length];
unsigned int tx_count=0;
int count=0;

// Single instance of the radio driver
 RH_NRF24 driver(7, 8);   // For RFM73 on Anarduino Mini

// Class to manage msg delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  // initalise serial
  Serial.begin(9600);
  Serial.println("Serial setup complete");
  if(!clientInit())//initiate the RF client
    Serial.print("RF init Failed!");
    
  // initialise sensors
  senseInit();
  
  // Finish setup
  Serial.println("Setup complete");
}



void loop()
{
  count++;
  //chech if the time slept is enough to call back
  if (count==CLIENT_ADDRESS*1){
   delay(50);
   //sense();
// DHT AIR T/RH SENSOR
  int chk = DHT.read11(DHT11_PIN);
  int dht_checksum_error=0x00; // variable to hold error codes
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
  AirTemp = DHT.temperature;
  AirHumidity = DHT.humidity;
  //AirTemp = 0x28;
  //Serial.println("T/RH measured");

  // Read PHOTOSENSOR values (16 bit integers)
  red = RGB_sensor.readRed();
  green = RGB_sensor.readGreen();
  blue = RGB_sensor.readBlue();

    // SOIL TEMPPERATURE
  sensors.requestTemperatures(); // Send the command to get temperatures
  SoilTemp = sensors.getTempC(soilThermometer);

  // SOIL MOISTURE
  digitalWrite(SoilVccPin, HIGH);   // enable sensor
  delay(50);  // allow power to settle
  unsigned int SoilMoisture = (1023 - (analogRead(SoilMoisturePin)))>>2; // read analog voltage then negate so that open circuit -> 0, short circtuit -> 1023.  Convert to 8 bits
  digitalWrite(SoilVccPin, LOW);   // disable sensor

  // increment tx count
   tx_count++;
   if (tx_count >= 65535){tx_count = 0;}

  // Construct msg
  msg[0] = (tx_count>>8) & 0xFF;
  msg[1] = tx_count & 0xFF;
  msg[2] = (red >> 8) & 0xFF;
  msg[3] = red & 0xFF;
  msg[4] = (green >> 8) & 0xFF;
  msg[5] = green & 0xFF;
  msg[6] = (blue >> 8) & 0xFF;
  msg[7] = blue & 0xFF;
  msg[8] = SoilMoisture & 0xFF;
  msg[9] = SoilTemp & 0xFF;
  msg[10] = AirTemp & 0xFF;
  msg[11] = AirHumidity & 0xFF;
  msg[12] = (dht_checksum_error>>8) & 0xFF;
  msg[13] = dht_checksum_error & 0xFF;

  
//  for (int i=0; i<sizeof(msg); i++){Serial.print(msg[i],HEX);Serial.print(",");}
//  Serial.print("\r\n");
//  Serial.println(msg);
 
   sendToServer(msg,sizeof(msg));
   count=0;
  delay(50);//small delay to allow all operations before sleeping again
  // Sleep with ADC module and BOD module off
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}


