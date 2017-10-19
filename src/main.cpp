#include <Homie.h>
#include <IPAddress.h>
#include "PZEM004T.h"

const int MEASURE_INTERVAL = 8;
unsigned long lastMeasureSent = 0;

HomieNode electrometerNode("measure", "measure");
HomieNode unitsNode("unit", "unit");

PZEM004T pzem(&Serial);
IPAddress ip(192,168,1,1);

void setupHandler() {
  IPAddress ipresp = pzem.setAddress(ip);
  unitsNode.setProperty("voltage").send("V");
  unitsNode.setProperty("current").send("A");
  unitsNode.setProperty("power").send("W");
  unitsNode.setProperty("energy").send("W/h");
  unitsNode.setProperty("ip").send(ipresp.toString());
}

void loopHandler() {
  if (millis() - lastMeasureSent >= MEASURE_INTERVAL * 1000UL || lastMeasureSent == 0) {
    // readMeasure();
    float v = pzem.voltage(ip);
    if (v < 0.0) v = 0.0;
    electrometerNode.setProperty("voltage").send(String(v));
    float i = pzem.current(ip);
    if(i >= 0.0){ electrometerNode.setProperty("current").send(String(i));}
    float p = pzem.power(ip);
    if(p >= 0.0){ electrometerNode.setProperty("power").send(String(p));}
    float e = pzem.energy(ip);
    if(e >= 0.0){ electrometerNode.setProperty("energy").send(String(e));}
    lastMeasureSent = millis();
  }
}

bool ipOnHandler(const HomieRange& range, const String& value) {
  if (ip.fromString(value)) {
    IPAddress ipresp = pzem.setAddress(ip);
    unitsNode.setProperty("ip").send(ipresp.toString());
    return true;
  } else {
    return false;
  }
}

void setup() {
  Serial.begin(9600);

  Homie_setFirmware("PZEM004T-meter", "1.0.0");
  Homie.disableLedFeedback();
  Homie.disableLogging();
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  electrometerNode.advertise("voltage");
  electrometerNode.advertise("current");
  electrometerNode.advertise("power");
  electrometerNode.advertise("energy");
  unitsNode.advertise("voltage");
  unitsNode.advertise("current");
  unitsNode.advertise("power");
  unitsNode.advertise("energy");
  unitsNode.advertise("ip").settable(ipOnHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
