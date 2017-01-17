// nrf24_reliable_datagram_server.pde
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>



#define SERVER_ADDRESS 1 //address of the server node
// Dont put this on the stack:
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

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

