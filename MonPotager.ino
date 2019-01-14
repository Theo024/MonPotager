#include <TheThingsNetwork.h>
#include <CayenneLPP.h>

const char *devAddr = "26011E5B";
const char *nwkSKey = "A3EC2462EE45CE6E39CD75D9081B391A";
const char *appSKey = "3C10E339A742B3DE7542AEB2172D092D";

#define loraSerial Serial2
#define debugSerial SerialUSB

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
CayenneLPP lpp(51);
uint8_t led = 0;
uint8_t led2 = 0;

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
  
  randomSeed(analogRead(0));

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  // Set callback for incoming messages
  ttn.onMessage(message);
}

void loop() {
  float temp = getTemperature();
  uint8_t presence = random(100);
  if (presence > 90) {
    presence = 1;
  } else {
    presence = 0;
  }

  debugSerial.println("-- LOOP");

  lpp.reset();
  lpp.addTemperature(1, temp);
  lpp.addRelativeHumidity(2, random(10000)/100.0);
  lpp.addBarometricPressure(3, random(92000, 108000)/100.0);
  lpp.addLuminosity(4, random(25000));
  lpp.addPresence(5, presence);
  lpp.addGPS(6, 45.18434, 5.75358, 300.05);
  lpp.addDigitalOutput(7, led);
  lpp.addDigitalOutput(8, led2);

  ttn.sendBytes(lpp.getBuffer(), lpp.getSize());

  delay(30000);
}

float getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.00 / 1023.00;
  float temp = (mVolts - 500.00) / 10.0;
  
  debugSerial.print((mVolts - 500) / 10);
  debugSerial.println(" Celcius");
  return float(temp);
}

void message(const byte *payload, size_t size, port_t port){
  debugSerial.println("-- MESSAGE");
  debugSerial.println(payload[0]);

  if (payload[0] == 7) {
    if (payload[2] > 0) {
      debugSerial.println("LED ON");
      led = 1;
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      debugSerial.println("LED OFF");
      led = 0;
      digitalWrite(LED_BUILTIN, LOW);
    }
  } else if (payload[0] == 8) {
    if (payload[2] > 0) {
      debugSerial.println("LED2 ON");
      led2 = 1;
      digitalWrite(LED_RED, LOW);
    } else {
      debugSerial.println("LED2 OFF");
      led2 = 0;
      digitalWrite(LED_RED, HIGH);
    }
  }

  
}
