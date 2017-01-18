// nrf24_reliable_datagram_server.pde
/*
 *   Message format (message length 20 bytes - max 28 bytes)
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
 */
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>



#define SERVER_ADDRESS 1 //address of the server node
// Dont put this on the stack:
#define msg_length 20 // length of message
uint8_t buf[msg_length]; //RH_NRF24_MAX_MESSAGE_LEN];

// Singleton instance of the radio driver
 RH_NRF24 driver(7, 8);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if(!serverInit())
    Serial.print("RF init Failed!");
  else
    Serial.print("RF initialised!");
}



void loop()
{
  checkMessages();
}

