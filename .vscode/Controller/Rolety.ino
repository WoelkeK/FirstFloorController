void rolety () {


  unsigned long currentMillis = millis();


  for (byte k = 0; k < noRelays3; k++) {
    if (debouncer3[k].update()) {
      int value3 = debouncer3[k].read();

      if ( value3 == LOW) {
        Relays3[k].buttonPushedMillis = currentMillis;
        Relays3[k].ROLReady = true;
      }
    }
   
    if (Relays3[k].ROLReady) {

      Relays4[k].ROLReady = false;
      if (!Relays4[k].relayState4) {
        Relays4[k].relayState4 = !Relays4[k].relayState4;
        digitalWrite(Relays4[k].relayPin4, Relays4[k].relayState4 ? RELAY_OFF : RELAY_ON);
        send(msg4[k].set(Relays4[k].relayState4 ? true : false));

      }

      // save sensor state in EEPROM (location == sensor number)

      if ((unsigned long)(currentMillis - Relays3[k].buttonPushedMillis > Relays3[k].turnOnDelay)) {

        Relays3[k].relayState3 = !Relays3[k].relayState3;
        digitalWrite(Relays3[k].relayPin3, Relays3[k].relayState3 ? RELAY_OFF : RELAY_ON);
        send(msg3[k].set(Relays3[k].relayState3 ? true : false));
        // save sensor state in EEPROM (location == sensor number)


        Relays3[k].ledTurnedOnAt = currentMillis;
        Relays3[k].ROLReady  = false;
      }
    }

    if (!Relays3[k].relayState3) {

      if ((unsigned long)(currentMillis - Relays3[k].ledTurnedOnAt) >= Relays3[k].turnOffDelay) {

        Relays3[k].relayState3 = !Relays3[k].relayState3;
        digitalWrite(Relays3[k].relayPin3, Relays3[k].relayState3 ? RELAY_OFF : RELAY_ON);
        send(msg3[k].set(Relays3[k].relayState3 ? true : false));
      }
    }

    


 if (debouncer4[k].update()) {
      int value4 = debouncer4[k].read();

      if ( value4 == LOW) {
        Relays4[k].buttonPushedMillis = currentMillis;
        Relays4[k].ROLReady = true;
      }
    }

    if (Relays4[k].ROLReady) {

      Relays3[k].ROLReady = false;
      if (!Relays3[k].relayState3) {

        Relays3[k].relayState3 = !Relays3[k].relayState3;
        digitalWrite(Relays3[k].relayPin3, Relays3[k].relayState3 ? RELAY_OFF: RELAY_ON);
        send(msg3[k].set(Relays3[k].relayState3 ? true : false));
      }


      // save sensor state in EEPROM (location == sensor number)

      if ((unsigned long)(currentMillis - Relays4[k].buttonPushedMillis > Relays4[k].turnOnDelay)) {

        Relays4[k].relayState4 = !Relays4[k].relayState4;
        digitalWrite(Relays4[k].relayPin4, Relays4[k].relayState4 ? RELAY_OFF : RELAY_ON);
        send(msg4[k].set(Relays4[k].relayState4 ? true : false));
        // save sensor state in EEPROM (location == sensor number)


        Relays4[k].ledTurnedOnAt = currentMillis;
        Relays4[k].ROLReady  = false;
      }
    }

    if (!Relays4[k].relayState4) {

      if ((unsigned long)(currentMillis - Relays4[k].ledTurnedOnAt) >= Relays4[k].turnOffDelay) {

        Relays4[k].relayState4 = !Relays4[k].relayState4;
        digitalWrite(Relays4[k].relayPin4, Relays4[k].relayState4 ? RELAY_OFF : RELAY_ON);
        send(msg4[k].set(Relays4[k].relayState4 ? true : false));
      }
      } 
 
    
}
}
 
