#include <Arduino.h>

#include "config.h"

typedef long long ll;
typedef unsigned long long ull;

double power = 0,
       target_speed = 10, // mm/s
       p, i, d,
       speed = 0, pulse_per_second = 0;

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

ull last_pulse=0, delta_time=0,
    timer_info=0, timer_pps=0;
double last_e = 0, e_sum=0;
void loop() {
  if(timer_pps < micros()-100000) {
    delta_time = micros() - timer_pps;
    timer_pps = micros();

    pulse_per_second = (pulse_count-last_pulse)*1e6/delta_time;
    last_pulse = pulse_count;

    // mm/s
    speed = GEAR_RATIO*WHEEL_O*pulse_per_second/MAGNET_ARM_C;
    double e = target_speed-speed;
    e_sum += e*delta_time;

    p = K_p * e;
    i = K_i * e_sum;
    d = K_d/delta_time * (e-last_e);
    last_e = e;

    power = constrain(power + p + i + d, 0, 1023);
  }

  if(timer_info < millis()-99) {
    // Serial.printf("[%8.4f] %llu count, %.3fms delta, %.3f pps, %.3fmm/s speed, %4d power\n", millis()/1000.0, pulse_count, delta_time/1e3, pulse_per_second, speed, power);
    Serial.printf("plot: %llu,%f,%f,%f,%d,%f\n", pulse_count, delta_time/1e3, pulse_per_second, speed, (int)power, target_speed);
    timer_info = millis();
  }

  analogWrite(AO_MOTOR_POW, (int)power);
}
