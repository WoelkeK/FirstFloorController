void inicjalizacja(){


  // initialize all the readings to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;

    
  
  mcp1.begin(addr1);
  mcp2.begin(addr2);
  mcp3.begin(addr3);

  
// Initialize Relays1 with corresponding buttons
  for (int i = 0; i < noRelays1; i++) {
    
    Relays1[i].buttonPin1 = buttonPin1[i];              // assign physical pins
    Relays1[i].relayPin1 = relayPin1[i];
    msg1[i].sensor = i;                                   // initialize messages
    msg1[i].type = V_LIGHT;
    
    if (i < 13){
    mcp1.pinMode(Relays1[i].buttonPin1, INPUT);
    mcp1.pullUp(Relays1[i].buttonPin1, HIGH);
    debouncer1[i] = BounceMcp();                        // initialize debouncer
    debouncer1[i].attach(mcp1, buttonPin1[i], 100);
    debouncer1[i].interval(50);
    
    
    
    
    }else if( i>12){
    mcp2.pinMode(Relays1[i].buttonPin1, INPUT);
    mcp2.pullUp(Relays1[i].buttonPin1, HIGH);
    debouncer1[i].attach(mcp2, buttonPin1[i], 100);
    }

    pinMode(Relays1[i].relayPin1, OUTPUT);
    Relays1[i].relayState1 = loadState(i);                               // retrieve last values from EEPROM
    digitalWrite(Relays1[i].relayPin1, Relays1[i].relayState1 ? RELAY_ON : RELAY_OFF); // and set relays accordingly
    send(msg1[i].set(Relays1[i].relayState1 ? true : false));                 // make controller aware of last status

    }
    

   



  //-------------------------------------------------------------------------------------------------------------------------------

  // Initialize Relays3 Rolety Góra
  for (int k = 0; k < noRelays3; k++) {
    Relays3[k].buttonPin3 = buttonPin3[k];              // assign physical pins
    Relays3[k].relayPin3 = relayPin3[k];
    msg3[k].sensor = k;                                   // initialize messages
    msg3[k].type = V_LIGHT;

    mcp3.pinMode(Relays3[k].buttonPin3, INPUT);
    mcp3.pullUp(Relays3[k].buttonPin3, HIGH);

 //   wait(100);
    pinMode(Relays3[k].relayPin3, OUTPUT);
    Relays3[k].relayState3 = true;                           // retrieve last values from EEPROM
    digitalWrite(Relays3[k].relayPin3, Relays3[k].relayState3 ? RELAY_OFF : RELAY_ON); // and set relays accordingly
    send(msg3[k].set(Relays3[k].relayState3 ? true : false));                 // make controller aware of last status
//    wait(50);

    debouncer3[k] = BounceMcp();                        // initialize debouncer
    debouncer3[k].attach(mcp3, buttonPin3[k], 100);
    debouncer3[k].interval(50);
  }


  // Initialize Relays4 Rolety dół
  for (int k = 0; k < noRelays4; k++) {
    Relays4[k].buttonPin4 = buttonPin4[k];              // assign physical pins
    Relays4[k].relayPin4 = relayPin4[k];
    msg4[k].sensor = k;                                   // initialize messages
    msg4[k].type = V_LIGHT;

    mcp3.pinMode(Relays4[k].buttonPin4, INPUT);
    mcp3.pullUp(Relays4[k].buttonPin4, HIGH);

 //   wait(100);
    pinMode(Relays4[k].relayPin4, OUTPUT);
    Relays4[k].relayState4 = true;                               // retrieve last values from EEPROM
    digitalWrite(Relays4[k].relayPin4, Relays4[k].relayState4 ? RELAY_OFF : RELAY_ON); // and set relays accordingly
    send(msg4[k].set(Relays4[k].relayState4 ? true : false));                 // make controller aware of last status
 //   wait(50);

    debouncer4[k] = BounceMcp();                        // initialize debouncer
    debouncer4[k].attach(mcp3, buttonPin4[k], 100);
    debouncer4[k].interval(50);
  }
}
  
