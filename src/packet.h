// -*- mode: cpp; eval: (platformio-mode 1); -*-
#ifndef PACKET_H_
#define PACKET_H_

#include <c_types.h>

#include "state.h"

/* Packet settings */
#define SOP '\x5e'
#define MAX_PACKET_SIZE 1024

typedef struct {
  uint16_t size;
  uint8_t version, type;
  char payload[MAX_PACKET_SIZE] = {0}, *head=payload;
} packet;

enum pv1_type {
  PV1_CONTROL=0,
  PV1_PID_COEFFICIENT_ALL,
  PV1_PID_TARGET_SPEED,
  PV1_SET_FLAGS,
  PV1_UNSET_FLAGS,
};

void parse_packet(packet&, state&);
void parse_packet_v1(packet&, state&);
void parse_char(packet&, int&, state&, char);

#endif // PACKET_H_
