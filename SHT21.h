#pragma once

#include <Arduino.h>
#include <Wire.h>

class Sht21 {
private:
  uint8_t mAddr;

public:
  void begin(uint8_t sda, uint8_t scl, uint8_t addr = 0x40);
  bool measure(float &temp, float &hum);
  bool measureT(float &result);
  bool measureRH(float &result);

protected:
  bool write(uint8_t cmd);
  bool read(uint16_t &result);
  bool checkCrc(const uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
};
