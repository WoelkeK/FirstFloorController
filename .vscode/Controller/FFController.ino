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