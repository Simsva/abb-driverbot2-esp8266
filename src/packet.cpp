// -*- eval: (platformio-mode 1); -*-
#include "packet.h"

#include <Arduino.h>

#include "endian.h"

void parse_packet_v1(packet& p, state& s) {
  switch(p.type) {
    case PV1_CONTROL:
      s.manual_speed = (int16_t)be16toh(*(uint16_t*)p.payload);
      s.manual_angle = *(uint8_t*)(p.payload+sizeof(int16_t));
      break;

    case PV1_PID_COEFFICIENT_ALL:
    {
      double *tmp = (double*)p.payload;
      s.kp = bereal64toh(tmp[0]);
      s.ki = bereal64toh(tmp[1]);
      s.kd = bereal64toh(tmp[2]);
    }
      break;

    case PV1_PID_TARGET_SPEED:
      s.auto_target_speed = bereal64toh(*(double*)p.payload);
      break;

    case PV1_SET_FLAGS:
      s.flags |= be32toh(*(uint32_t*)p.payload);
      break;

    case PV1_UNSET_FLAGS:
      s.flags &= ~be32toh(*(uint32_t*)p.payload);
      break;

    default:
      Serial.println("Invalid packet type");
      break;
  }
}

void parse_packet(packet& p, state& s) {
  Serial.printf("s:%u v:%u t:%u p:(%s)\n", p.size, p.version, p.type, p.payload);

  switch(p.version) {
    case 1:
      parse_packet_v1(p, s);
      break;

    default:
      Serial.println("Invalid packet version");
      break;
  }
}

/*
** Packet format:
**  1 byte     - SOP
**  1 byte     - Version (unsigned)
**  1 byte     - Type (unsigned)
**  2 byte     - Size (unsigned big endian)
**  Size bytes - Payload
 */
void parse_char(packet& p, int& bytes_read, state& s, char ch) {
  Serial.printf("Recv char: %c (%02x)", ch, ch);
  if(bytes_read == 0) {
    if(ch == SOP) {
      Serial.println(" sop");
      p.head = p.payload;
    } else {
      Serial.println(" nul");
      --bytes_read;
    }
  } else if(bytes_read == 1) {
    Serial.println(" ver");
    p.version = ch;
  } else if(bytes_read == 2) {
    Serial.println(" typ");
    p.type = ch;
  } else if(bytes_read < 5) {
    Serial.println(" siz");
    ((char*)&p.size)[bytes_read-3] = ch;
    if(bytes_read == 4) {
      // Convert from big endian
      p.size = be16toh(p.size);
      Serial.printf("Packet size: %d\n", p.size);

      if(p.size > MAX_PACKET_SIZE-1) {
        Serial.println("Packet size is too big");
        p.head = p.payload;
        p.size = 0;
        bytes_read = 0;
        return;
      }
    }
  } else {
    *(p.head++) = ch;
    if(bytes_read - 4 >= p.size) {
      Serial.println(" las");
      bytes_read = 0;
      *p.head = '\0';
      parse_packet(p, s);
      return;
    } else {
      Serial.println(" pay");
    }
  }
  ++bytes_read;
}
