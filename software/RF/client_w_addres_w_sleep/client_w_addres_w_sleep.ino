#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 3


//RF stuff
#define CLIENT_ADDRESS 2 //self address
#define SERVER_ADDRESS 1 //address of the server node
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

int count=0;

// Singleton instance of the radio driver
 RH_NRF24 driver(7, 8);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Serial setup complete");
  if(!clientInit())//initiate the RF client
    Serial.print("RF init Failed!");
//  senseInit();
}



void loop()
{
    // Try again 1s later
    delay(1000);
  String message="new message2";
  count++;
  //chech if the time slept is enough to call back
  if (count==CLIENT_ADDRESS*1){
   delay(50);
   //sense();
   sendToServer(message);
   count=0;
   delay(500);//small delay to allow all operations before sleeping again
  }
}


