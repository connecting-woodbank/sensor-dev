int clientInit()
{
   if (!manager.init())// Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm 
    return 0;
   else
    return 1;
}

int sendToServer(String message){
  uint8_t data[message.length()];
  message.toCharArray(data,message.length()+1);
  // Send a message to manager_server
  if (manager.sendtoWait(data, message.length()+1, SERVER_ADDRESS))//using the lenght of the string because the buffer is not clean on startup or message change
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is the server running?");
    }
  }
  else
    Serial.println("send failed");

   return 1;
  }
