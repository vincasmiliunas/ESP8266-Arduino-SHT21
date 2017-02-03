#include <SHT21.h>

#define DEBUG 1

Sht21 sht21;

void setup() {
  Serial.begin(115200);
  // The default is (SDA, SCL) == (D2, D1) == (4, 5)
  // For ESP-01 use (0, 2)
  sht21.begin(SDA, SCL);
}

void loop() {
  float temp, hum;
  if (sht21.measure(temp, hum)) {
    Serial.printf("Temperature = %0.2fC, Humidity = %0.2f%%\n", temp, hum);
  } else {
    Serial.printf("Measurement failed.\n");
  }

  delay(1000);
}
