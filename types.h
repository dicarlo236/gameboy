#ifndef GBC_TYPES_H
#define GBC_TYPES_H

#include <stdint.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;

struct KeyState {
  bool a,b,u,d,l,r,start,select,turbo;
};

extern KeyState keyboard;

#endif //GBC_TYPES_H
