void pomiar (){

  // calculate average
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(LIGHT_SENSOR_ANALOG_PIN)/10;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  
  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  average = total / numReadings; // average
  
  Serial.println(average); // whatever is printed here is sent to UNITY
  delay(1); // delay in between reads for stability
  
  if (average!= lastaverage) {
    send(msg.set(average));
    lastaverage = average;
    delay(10);
  
  }
}



// //*********************************************************************
//  // Czułość fotosensora dla schodów
//
//
//  if (lightLevel < 50) {
//    detektor = false;
//
//  } else {
//    if (lightLevel> 100) {
//      detektor = true;
//    }
//  }
//
//  //*********************************************************************
//  // Czułość fotosensora dla rolety łazienka
//
//
//  if (lightLevel < 100) {
//    detektor2 = false;
//
//  } else {
//    if (lightLevel > 800) {
//      detektor2 = true;
//    }
//  }
//  //***********************************************************************
//
  
 
