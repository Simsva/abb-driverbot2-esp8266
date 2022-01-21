// -*- mode: cpp; eval: (platformio-mode 1); -*-
#ifndef ENDIAN_H_
#define ENDIAN_H_

// Pretty much copied from endian.h

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __PDP_ENDIAN 3412

#include "endianness.h"

#include <c_types.h>

static inline uint16 __uint16_identity(uint16 x) {
  return x;
}

static inline uint32 __uint32_identity(uint32 x) {
  return x;
}

static inline uint64 __uint64_identity(uint64 x) {
  return x;
}

// Copied straight from byteswap.h
#define __bswap_16(x) \
  ((__uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

#define __bswap_32(x) \
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8) \
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

#define __bswap_64(x) \
  ((((x) & 0xff00000000000000ull) >> 56) \
   | (((x) & 0x00ff000000000000ull) >> 40) \
   | (((x) & 0x0000ff0000000000ull) >> 24) \
   | (((x) & 0x000000ff00000000ull) >> 8) \
   | (((x) & 0x00000000ff000000ull) << 8) \
   | (((x) & 0x0000000000ff0000ull) << 24) \
   | (((x) & 0x000000000000ff00ull) << 40) \
   | (((x) & 0x00000000000000ffull) << 56))

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define htobe16(x) __bswap_16 (x)
# define htole16(x) __uint16_identity (x)
# define be16toh(x) __bswap_16 (x)
# define le16toh(x) __uint16_identity (x)

# define htobe32(x) __bswap_32 (x)
# define htole32(x) __uint32_identity (x)
# define be32toh(x) __bswap_32 (x)
# define le32toh(x) __uint32_identity (x)

# define htobe64(x) __bswap_64 (x)
# define htole64(x) __uint64_identity (x)
# define be64toh(x) __bswap_64 (x)
# define le64toh(x) __uint64_identity (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
# define htobe16(x) __uint16_identity (x)
# define htole16(x) __bswap_16 (x)
# define be16toh(x) __uint16_identity (x)
# define le16toh(x) __bswap_16 (x)

# define htobe32(x) __uint32_identity (x)
# define htole32(x) __bswap_32 (x)
# define be32toh(x) __uint32_identity (x)
# define le32toh(x) __bswap_32 (x)

# define htobe64(x) __uint64_identity (x)
# define htole64(x) __bswap_64 (x)
# define be64toh(x) __uint64_identity (x)
# define le64toh(x) __bswap_64 (x)
#else
# error "Change your endianness to a supported value in 'endianness.h'"
#endif

// Doubles
union real_int64 {
  uint64_t d;
  real64 f;
};

static inline real64 bereal64toh(real64 x) {
  real_int64 tmp;
  tmp.f = x;
  tmp.d = be64toh(tmp.d);
  return tmp.f;
}

static inline real64 lereal64toh(real64 x) {
  real_int64 tmp;
  tmp.f = x;
  tmp.d = le64toh(tmp.d);
  return tmp.f;
}

#endif // ENDIAN_H_
