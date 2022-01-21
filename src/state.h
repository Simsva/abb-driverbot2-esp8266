// -*- mode: cpp; eval: (platformio-mode 1); -*-
#ifndef STATE_H_
#define STATE_H_

enum state_flags {
  STATE_MANUAL_CONTROL=1,
};

typedef struct state {
  double auto_target_speed = 0, /* mm/s */
         kp = 0, ki = 0, kd = 0;

  int16_t manual_speed = 0;
  uint8_t manual_angle = 90;

  unsigned int flags = STATE_MANUAL_CONTROL;
} state;

#endif // STATE_H_
