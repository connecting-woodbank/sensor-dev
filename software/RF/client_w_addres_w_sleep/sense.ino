//void senseInit(){
//  pinMode(A1, OUTPUT);
//  digitalWrite(A1, HIGH);
//  //dht.begin();
//  }
//
//void sense(){
//  //sense soil temp (check)
//  
//  //sense ambient light
//
//  //sense soil humudity (check)
//
//  // Get temperature event and print its value.
//  DHT.read11(DHT11_PIN);
//  Serial.print(DHT.humidity, 1);
//  Serial.print(",\t");
//  Serial.println(DHT.temperature, 1);
//  sensors_event_t event;  
//  dht.temperature().getEvent(&event);
//  if (isnan(event.temperature)) {
//    Serial.println("Error reading temperature!");
//    }
//  else {
//    Serial.print("Temperature: ");
//    Serial.print(event.temperature);
//    Serial.println(" *C");
//    }
//
//   // Get humidity event and print its value.
//  dht.humidity().getEvent(&event);
//  if (isnan(event.relative_humidity)) {
//    Serial.println("Error reading humidity!");
//    }
//  else {
//    Serial.print("Humidity: ");
//    Serial.print(event.relative_humidity);
//    Serial.println("%");
//    }
//    
//  }
