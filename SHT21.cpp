#include "SHT21.h"

#if DEBUG
#define dprintf(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
#define dprintf(format, ...)
#endif

#define COMMAND_MEASURE_T_HOLD 0b11100011
#define COMMAND_MEASURE_RH_HOLD 0b11100101

void Sht21::begin(uint8_t sda, uint8_t scl, uint8_t addr) {
  Wire.begin(sda, scl);
  mAddr = addr;
}

bool Sht21::measure(float &temp, float &hum) {
  return measureT(temp) && measureRH(hum);
}

bool Sht21::measureT(float &result) {
  if (!write(COMMAND_MEASURE_T_HOLD)) {
    return false;
  }
  delay(85);
  uint16_t value;
  if (!read(value)) {
    return false;
  }
  result = -46.85 + (175.72 / 65536.0) * value;
  return true;
}

bool Sht21::measureRH(float &result) {
  if (!write(COMMAND_MEASURE_RH_HOLD)) {
    return false;
  }
  delay(29);
  uint16_t value;
  if (!read(value)) {
    return false;
  }
  result = -6.0 + (125.0 / 65536.0) * value;
  return true;
}

bool Sht21::write(uint8_t cmd) {
  Wire.beginTransmission(mAddr);
  const auto l = Wire.write(cmd);
  if (l != 1) {
    dprintf("Wire.write(%x) failed\n", cmd);
    return false;
  }
  const auto r = Wire.endTransmission();
  if (r != 0) {
    dprintf("Wire.endTransmission failed, err = %d\n", r);
    return false;
  }
  return true;
}

bool Sht21::read(uint16_t &result) {
  const auto l = Wire.requestFrom(mAddr, (uint8_t)3);
  if (l != 3) {
    dprintf("Wire.requestFrom failed, len = %d\n", l);
    return false;
  }
  const uint8_t data[2] = {(uint8_t)Wire.read(), (uint8_t)Wire.read()};
  const uint8_t checksum = Wire.read();
  if (!checkCrc(data, 2, checksum)) {
    dprintf("checkCrc failed, data = [%x, %x] checksum = %x\n", data[0],
            data[1], checksum);
    return false;
  }
  result = (uint16_t(data[0]) << 8) | (uint16_t(data[1]) & 0b11111100);
  return true;
}

// https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/11_Sample_Codes_Software/Humidity_Sensors/Sensirion_Humidity_Sensors_SHT21_Sample_Code_V1.2.pdf
bool Sht21::checkCrc(const uint8_t data[], uint8_t nbrOfBytes,
                     uint8_t checksum) {
  uint8_t crc = 0;
  uint8_t byteCtr;
  // calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
    crc ^= (data[byteCtr]);
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80)
        crc = (crc << 1) ^ (uint16_t)0x131;
      else
        crc = (crc << 1);
    }
  }
  return crc == checksum;
}
