/* DHT library 

MIT license
written by Adafruit Industries
*/

#include "DHT.h"

DHT::DHT(uint8 pin, uint8 type) {
  _pin = pin;
  _type = type;
  firstreading = true;
}

void DHT::begin(void) {
  // set up the pins!
  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH);
  _lastreadtime = 0;
  lastTemperature = NAN;
  lastHumidity = NAN;
}

//boolean S == Scale.  True == Farenheit; False == Celcius
float DHT::readTemperature(bool S) {
  float f;

  if (read()) {
    switch (_type) {
    case DHT11:
      f = lastTemperature = data[2];
      if(S)
      	f = convertCtoF(f);
      	
      return f;
    case DHT22:
    case DHT21:
      f = data[2] & 0x7F;
      f *= 256;
      f += data[3];
      f /= 10;
      if (data[2] & 0x80)
		f *= -1;
      lastTemperature = f;
      if(S)
		f = convertCtoF(f);

      return f;
    }
  }
  if (S)
    return convertCtoF(lastTemperature);
  return lastTemperature;
}

float DHT::convertCtoF(float c) {
	return c * 9 / 5 + 32;
}

float DHT::readHumidity(void) {
  float f;
  if (read()) {
    switch (_type) {
    case DHT11:
      f = lastHumidity = data[0];
      return f;
    case DHT22:
    case DHT21:
      f = data[0];
      f *= 256;
      f += data[1];
      f /= 10;
      lastHumidity = f;
      return f;
    }
  }
  return lastHumidity;
}

uint8 DHT::waitFor(uint8 val, uint8 pin, uint8 timeout_us){
	uint32 start = micros();
	uint32 timeout_t = start + timeout_us;
	while(digitalRead(pin) != val){
		if(micros() > timeout_t) return 0xff;
	}
	return micros() - start;
}

uint8 DHT::WAIT_FOR(uint8 val, uint8 timeout_us, uint8 error)
{
  			uint8 __r = waitFor(val, _pin, timeout_us);
			if(__r == 0xff) return error;
			__r;
}
  
boolean DHT::read(void) {
  uint8 j = 0;
  unsigned long currenttime;

  currenttime = millis();
  if (currenttime < _lastreadtime) {
    // ie there was a rollover
    _lastreadtime = 0;
  }
  if (!firstreading && ((currenttime - _lastreadtime) < 2000)) {
    return true; // return last correct measurement
    //delay(2000 - (currenttime - _lastreadtime));
  }
  firstreading = false;
  
  _lastreadtime = millis();

  data[0] = data[1] = data[2] = data[3] = data[4] = 0;
  
  digitalWrite(_pin, LOW); // externally pulled up: don't use internal pullup

  // now pull it low for ~20 milliseconds
  pinMode(_pin, OUTPUT);
  delay(20);
  pinMode(_pin, INPUT);
  delayMicroseconds(40);

  // sensor will respond by pulling low for 80us and high for 80us
  WAIT_FOR(HIGH, 90, 1);
  WAIT_FOR(LOW,  90, 2);

  for(int i = 0; i < 40; ++i){
	  uint8* byte = &data[i/8];
	  *byte <<= 1;

	  // then each bit is 50us of low, and then either 25 or 70 us of high (0/1)
	  WAIT_FOR(HIGH, 60, 3);
	  uint8 elapsed = WAIT_FOR(LOW, 90, 4);
	  if(elapsed > 40){
		  *byte |= 0x1;
	  }
  }
  
  // check we read 40 bits and that the checksum matches
  if ((j >= 40) && 
      (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
    return true;
  }
  
  return false;

}
