//
// Created by jared on 1/10/19.
//

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
  int toInt() {
    return a + 2*b + 4*u + 8*d + 16*l + 32*r + 64*start + 128*select;
  }
};

extern KeyState keyboard;
extern u32 newCycles;

#endif //GBC_TYPES_H
