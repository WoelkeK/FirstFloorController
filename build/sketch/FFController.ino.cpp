#include <Arduino.h>
#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\FFController.ino"
// Pierwsza wersja sterowania lokalnego plus feedback i odczyt z Domoticz
// Działające światła, plus obsługa satela, rolety sterowane prawidłowo
// Kolejna modyfikacja - dołączenie pomiaru swiatła na zewnątrz 18/11/19 22:55

// Enable debug prints to serial monitor

#define MY_DEBUG

#define MY_RS485
// Define this to enables DE-pin management on defined pin
#define MY_RS485_DE_PIN 15
// Set RS485 baud rate to use
#define MY_RS485_BAUD_RATE 19200
// Enable this if RS485 is connected to a hardware serial port
#define MY_RS485_HWSERIAL Serial2

#define MY_NODE_ID 1

//#define MY_TRANSPORT_WAIT_READY_MS 5000        //set how long to wait for transport ready in milliseconds
// Enable and select rs 485 tyransport layer

#include <MySensors.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

#include <Bounce2mcp.h>
//#include <Bounce2.h>

Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;

const byte addr1 = 0x00; // uwaga zamienionyu adres z 3
const byte addr2 = 0x01;
const byte addr3 = 0x03;

#define SN "Sterownik Dolny"
#define SV "1.0"

// Czujnik  Światła

#define CHILD_ID_LIGHT 50
#define LIGHT_SENSOR_ANALOG_PIN A0

uint32_t SLEEP_TIME = 30 * 1000; // Sleep time between reads (in milliseconds)
const int numReadings = 100;     // Define the number of samples to keep track of.
int readings[numReadings];       // the readings from the analog input
int readIndex = 0;               // the index of the current reading
int total = 0;                   // the running total
int average = 0;
int lastaverage = 0;
MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);

bool detektor = false;
bool detektor2 = false;

#define RELAY_ON 0 // Ustawienie polaryzacji dla przekaźników ACTIVE LOW / ACTIVE HIGH
#define RELAY_OFF 1
//

#define noRelays1 20
#define noRelays2 7
#define noRelays3 7
#define noRelays4 7

const int relayPin1[] = {37, 43, 8, A14, 45, 34, 35, 46, 49, 7, A12, 36, 47, 41, 40, 39, 42, A13, 44, 38};
const int buttonPin1[] = {13, 12, 11, 10, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 8};

// Przekaźniki rolety Góra podpięte do 3 expandera MCP3

const int relayPin3[] = {9, 27, 23, 25, 11, 31, 33}; // wyjscia na przekazniki roleta gora
const int buttonPin3[] = {15, 12, 10, 8, 7, 2, 1};   // przyciski roleta gora

// Przekaźniki rolety Dół podpięte do 3 expandera MCP3

const int relayPin4[] = {10, 26, 22, 24, 12, 30, 32}; // wyjscia na przekaźniki roleta dol
const int buttonPin4[] = {14, 13, 11, 9, 6, 3, 0};    // przyciski roleta dol

class Relay1 // relay class, store all relevant data (equivalent to struct)
{
public:
  int buttonPin1;      // physical pin number of button
  int relayPin1;       // physical pin number of relay
  boolean relayState1; // relay status (also stored in EEPROM)
};

class Relay2 // relay class, store all relevant data (equivalent to struct)
{
public:
  int buttonPin2;      // physical pin number of button
  int relayPin2;       // physical pin number of relay
  boolean relayState2; // relay status (also stored in EEPROM)
};
class Relay4 // relay class, store all relevant data (equivalent to struct)
{
public:
  int buttonPin4;                     // physical pin number of button
  int relayPin4;                      // physical pin number of relay
  boolean relayState4;                // relay status (also stored in EEPROM)
  bool ROLReady = false;              // flag for when button is let go
  unsigned long buttonPushedMillis;   // when button was released
  unsigned long ledTurnedOnAt;        // when led was turned on
  unsigned long turnOnDelay = 500;    // wait to turn on LED
  unsigned long turnOffDelay = 23000; // turn off LED after this time
};
class Relay3 // relay class, store all relevant data (equivalent to struct)
{
public:
  int buttonPin3;                     // physical pin number of button
  int relayPin3;                      // physical pin number of relay
  boolean relayState3;                // relay status (also stored in EEPROM)
  unsigned long buttonPushedMillis;   // when button was released
  unsigned long ledTurnedOnAt;        // when led was turned on
  unsigned long turnOnDelay = 500;    // wait to turn on LED
  unsigned long turnOffDelay = 30000; // turn off LED after this time
  unsigned long debounce = 10;
  bool ROLReady = false; // flag for when button is let go
};

Relay1 Relays1[noRelays1];
BounceMcp debouncer1[noRelays1];
MyMessage msg1[noRelays1];
Relay2 Relays2[noRelays2];
BounceMcp debouncer2[noRelays2];
MyMessage msg2[noRelays2];
Relay3 Relays3[noRelays3];
BounceMcp debouncer3[noRelays3];
MyMessage msg3[noRelays3];
Relay4 Relays4[noRelays4];
BounceMcp debouncer4[noRelays4];
MyMessage msg4[noRelays4];

/*
  void before() {
  for (int sensor=1, pin=RELAY_1; sensor<=NUMBER_OF_RELAYS;sensor++, pin++) {
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);
    // Set relay to last known state (using eeprom storage)
    digitalWrite(pin, loadState(sensor)?RELAY_ON:RELAY_OFF);
  }
  }*/

#line 143 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\FFController.ino"
void setup();
#line 156 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\FFController.ino"
void presentation();
#line 171 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\FFController.ino"
void loop();
#line 201 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\FFController.ino"
void receive(const MyMessage &message);
#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Pomiary.ino"
void pomiar();
#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Rolety.ino"
void rolety();
#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Setup.ino"
void inicjalizacja();
#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Swiatla.ino"
void swiatla();
#line 143 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\FFController.ino"
void setup()
{

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //  wait(100);

  inicjalizacja();

  //  wait(50);
}
//**********************************************************************************************************************************************************
void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SN, SV);

  // wait(100);
  present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);

  for (int i = 0; i < noRelays1; i++)
    present(i, S_LIGHT); // present sensor to gateway

  //  wait(100);
}

//***************************************************************************************************************
void loop()
{

  pomiar();
  swiatla();
  rolety();

  // if ((millis() - last_heartbeat_time) > HEARTBEAT_TIME) {
  //  // If it exceeds the heartbeat time then send a heartbeat
  //  heartbeat = !heartbeat;
  //  digitalWrite(13, heartbeat);
  //  last_heartbeat_time = millis();
  //
  //  #ifdef MY_DEBUG
  //   if (heartbeat){
  //    Serial.println("heartbeat ON");
  //   }else{
  //   Serial.println("heartbeat OFF");
  //
  //   }
  //  #endif
  //}

  // wait(UPDATE_TIME);

  //  wait(20);
}

//***************************************************************************************************************

void receive(const MyMessage &message)
{

  if (message.type == V_LIGHT)
  {
    // Change relay state

    Relays1[message.sensor].relayState1 = message.getBool();

    digitalWrite(Relays1[message.sensor].relayPin1, Relays1[message.sensor].relayState1 ? RELAY_ON : RELAY_OFF); // and set relays accordingly;
    // Store state in eeprom
    saveState(message.sensor, Relays1[message.sensor].relayState1);

    // Write some debug info
    Serial.print("Incoming change for sensor:");
    Serial.print(message.sensor);
    Serial.print(", New status: ");
    Serial.println(message.getBool());

    //
    //        Relays2[message.sensor].relayState2 = message.getBool();
    //        digitalWrite(Relays2[message.sensor].relayPin2, Relays2[message.sensor].relayState2 ? RELAY_ON : RELAY_OFF); // and set relays accordingly
    //        saveState( message.sensor, Relays2[message.sensor].relayState2 );
    //        break;
    //
    //      case 3:
    //
    //        Relays3[message.sensor].relayState3 = message.getBool();
    //        digitalWrite(Relays3[message.sensor].relayPin3, Relays3[message.sensor].relayState3 ? RELAY_ON : RELAY_OFF); // and set relays accordingly
    //        saveState( message.sensor, Relays3[message.sensor].relayState3 );
    //        break;
    //
    //
    //      case 4:
    //
    //        Relays4[message.sensor].relayState4 = message.getBool();
    //        digitalWrite(Relays4[message.sensor].relayPin4, Relays4[message.sensor].relayState4 ? RELAY_ON : RELAY_OFF); // and set relays accordingly
    //        saveState( message.sensor, Relays4[message.sensor].relayState4 );
    //        break;
    //
    //
  }
}
#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Pomiary.ino"
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
  
 

#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Rolety.ino"
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
 

#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Setup.ino"
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
  

#line 1 "c:\\Users\\Cris\\VSCode\\FirstFloorController\\.vscode\\Controller\\Swiatla.ino"
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

