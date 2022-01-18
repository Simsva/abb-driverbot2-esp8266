#include <Arduino.h>

#include "config.h"

typedef long long ll;
typedef unsigned long long ull;

ull pulse_count = 0;
void IRAM_ATTR on_pulse() {
  ++pulse_count;
}

void setup() {
  pinMode(DO_SERVO, OUTPUT);
  pinMode(DO_MOTOR_DIR, OUTPUT);
  pinMode(AO_MOTOR_POW, OUTPUT);

  pinMode(DI_HALL, INPUT);

  attachInterrupt(digitalPinToInterrupt(DI_HALL), on_pulse, RISING);

  Serial.begin(115200);
}

void loop() {
  Serial.printf("[%8.4f] %llu count\n", millis()/1000.0, pulse_count);
  delay(100);
}
