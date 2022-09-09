void swiatla () {



  for (byte i = 0; i < noRelays1; i++) {

    if (debouncer1[i].update()) {

      int value1 = debouncer1[i].read();

      if ( value1 == LOW) {
        Relays1[i].relayState1 = !Relays1[i].relayState1;
        digitalWrite(Relays1[i].relayPin1, Relays1[i].relayState1 ? RELAY_ON : RELAY_OFF);
        send(msg1[i].set(Relays1[i].relayState1 ? true : false));
        // save sensor state in EEPROM (location == sensor number)
        saveState( i, Relays1[i].relayState1 );

      }
    }
  }
}
