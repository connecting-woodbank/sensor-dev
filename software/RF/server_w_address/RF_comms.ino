int serverInit(){
  if (!manager.init())
    return 0;
  else
    return 1;
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  }

 void checkMessages(){
    if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from,HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      String message="Hello back client " + String(from);
      uint8_t data[message.length()];
      message.toCharArray(data,message.length()+1);
      
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, message.length()+1, from))
        Serial.println("sendtoWait failed");
    }
  }
  
  }
