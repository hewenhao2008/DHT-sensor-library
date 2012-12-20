#ifndef DHT_H
#define DHT_H

#if ARDUINO >= 100
 #include "Arduino.h"
#elif defined(__ARM3X8E__)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/* DHT library (maple port)

MIT license
written by Adafruit Industries
*/

#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

class DHT {
 private:
  uint8 data[6];
  uint8 _pin, _type;
  boolean read(void);
  unsigned long _lastreadtime;
  boolean firstreading;
  float lastTemperature, lastHumidity;
  uint8 waitFor(uint8 val, uint8 pin, uint8 timeout_us);
  uint8 WAIT_FOR(uint8 val, uint8 timeout_us, uint8 error);


 public:
  DHT(uint8 pin, uint8 type);
  void begin(void);
  float readTemperature(bool S=false);
  float convertCtoF(float);
  float readHumidity(void);

};
#endif

        