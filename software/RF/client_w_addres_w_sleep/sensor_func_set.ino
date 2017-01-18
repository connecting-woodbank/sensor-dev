void senseInit(){
  // DHT AIR T/RH SENSOR
  int chk = DHT.read11(DHT11_PIN);
  int dht_checksum_error=0xFFFF; // variable to hold error codes
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.println("DHT OK");
      dht_checksum_error = 0;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      dht_checksum_error |= 0x01;
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      dht_checksum_error |= 0x02;
      break;
    case DHTLIB_ERROR_CONNECT:
      Serial.print("Connect error,\t");
      dht_checksum_error |= 0x04;
      break;
    case DHTLIB_ERROR_ACK_L:
      Serial.print("Ack Low error,\t");
      dht_checksum_error |= 0x08;
      break;
    case DHTLIB_ERROR_ACK_H:
      Serial.print("Ack High error,\t");
      dht_checksum_error |= 0x10;
      break;
    default:
      Serial.print("Unknown error,\t");
      dht_checksum_error |= 0x80;
      break;
  }

  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Light sensor Initialization Successful\n\r");
  }
  
    // initialise dallas (soil) temperature sensor
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

    
    // initialise soil moisture sensor
    pinMode(SoilVccPin, OUTPUT);  // Vcc pin for soil moisture sensor
    digitalWrite(SoilVccPin, LOW);   // disable sensor
 }

