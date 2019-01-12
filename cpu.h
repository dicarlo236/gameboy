//
// Created by jared on 1/10/19.
//

#ifndef GBC_CPU_H
#define GBC_CPU_H

#include "types.h"

union reg {
  u16 v;
  struct {
    u8 lo;
    u8 hi;
  };
};

struct CpuState {
  //reg af, bc, de, hl, sp, pc;
  reg bc, de, hl;
  u8 f;
  u8 a;
  u16 sp, pc;
  bool stop, halt;
  uint64_t cycleCount;
  uint64_t divOffset;
  u16 div;
  u8 ime;
};

extern CpuState globalState;

void updateKeypadRegister(KeyState* keys);

void resetCpu();

void cpuStep();

void printCpuState();

bool getZeroFlag();
bool getSubtractFlag();
bool getHalfCarryFlag();
bool getCarryFlag();

void setZeroFlag();
void clearZeroFlag();
void setSubtractFlag();
void clearSubtractFlag();
void setHalfCarryFlag();
void clearHalfCarryFlag();
void setCarryFlag();
void clearCarryFlag();

void clearAllFlags();

#endif //GBC_CPU_H
