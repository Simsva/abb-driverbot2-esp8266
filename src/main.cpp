// -*- eval: (platformio-mode 1); -*-
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

#include "state.h"
#include "packet.h"

#include "config.h"

/* Types */
typedef long long ll;
typedef unsigned long long ull;

/* Globals */
Servo servo;

WiFiServer server(TCP_PORT);
WiFiClient client;

state s;
packet p;
int p_read = 0;

double power = 0,
       vp, vi, vd,
       speed = 0, pulse_per_second = 0,
       last_e = 0, e_sum=0;

ull pulse_count = 0, last_pulse = 0, delta_time = 0,
    timer_info = 0, timer_calc = 0;

/* Code */
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

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("\nConnected: %s\n", WiFi.localIP().toString().c_str());

  server.begin();

  servo.attach(DO_SERVO);
}

void loop() {
  if(timer_calc < micros()-1e5 && !(s.flags & STATE_MANUAL_CONTROL)) {
    delta_time = micros() - timer_calc;
    timer_calc = micros();

    pulse_per_second = (pulse_count-last_pulse)*1e6/delta_time;
    last_pulse = pulse_count;

    // mm/s
    speed = GEAR_RATIO*WHEEL_CIRCUMFERENCE*pulse_per_second/MAGNET_ARM_C;
    double e = s.auto_target_speed-speed;
    e_sum += e*delta_time;

    vp = s.kp * e;
    vi = s.ki * e_sum;
    vd = s.kd/delta_time * (e-last_e);
    last_e = e;

    power = constrain(power + vp + vi + vd, 0, 1023);
  }

  if(timer_info < millis()-99) {
    // Serial.printf("[%8.4f] %llu count, %.3fms delta, %.3f pps, %.3fmm/s speed, %4d power\n", millis()/1000.0, pulse_count, delta_time/1e3, pulse_per_second, speed, power);
    Serial.printf("plot: %llu,%f,%f,%f,%d,%f\n", pulse_count, delta_time/1e3, pulse_per_second, speed, (int)power, s.auto_target_speed);
    Serial.printf("speed=%4d (%d) angle=%3d kp=%6.3f ki=%6.3f kd=%6.3f\n",
                  s.manual_speed, s.manual_speed<0, s.manual_angle, s.kp, s.ki, s.kd);
    timer_info = millis();
  }

  if(!client) client = server.available();
  else {
    if(client.connected()) {
      while(client.available()>0) parse_char(p, p_read, s, client.read());
    } else {
      Serial.println("Client disconnected");
      client.stop();
      // Read buffered chars
      while(client.available()>0) parse_char(p, p_read, s, client.read());
    }
  }

  if(s.flags & STATE_MANUAL_CONTROL) {
    digitalWrite(DO_MOTOR_DIR, s.manual_speed < 0);
    analogWrite(AO_MOTOR_POW, abs(s.manual_speed));

    servo.write(s.manual_angle);
  } else {
    analogWrite(AO_MOTOR_POW, (int)power);
  }
}
