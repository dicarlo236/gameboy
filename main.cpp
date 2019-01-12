#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "platform.h"
#include "types.h"
#include "mem.h"
#include "cpu.h"
#include "graphics_display.h"
#include "video.h"

// number of Z80 instructions / per keyboard update
#define INSTR_PER_KEYCHECK 200

// global keyboard
KeyState keyboard;

// convert a keyboard state to a unique integer.
int keyboardToInt(KeyState* k) {
  return k->a + 2*k->b + 4*k->u + 8*k->d + 16*k->l + 32*k->r + 64*k->start + 128*k->select;
}

int main(int argc, char** argv) {
  if(argc != 2) {
    return 0;
  }

  // load game
  FileLoadData file = loadFile(argv[1]);

  // set up gameboy
  initMem(file);
  initVideo();
  resetCpu();

  // set up graphics
  initGraphics(globalVideoState.frameBuffer);
  updateGraphics();

  // set up keyboard
  updateKeyboard(&keyboard);
  int lastKeyboardInt = keyboardToInt(&keyboard);

  // main loop
  for(;;) {
    // update keyboard
    updateKeyboard(&keyboard);

    // if the keyboard has changed, generate a key interrupt
    int currentKeyboardInt = keyboardToInt(&keyboard);
    if(lastKeyboardInt != currentKeyboardInt) {
      globalMemState.ioRegs[IO_IF] |= INTERRUPT_KEY;

    }
    lastKeyboardInt = currentKeyboardInt;


    // step the CPU / Video systems as much as requested
    for(int i =  0; i < INSTR_PER_KEYCHECK; i++) {
      u32 cpuCycles = cpuStep();
      stepVideo(cpuCycles);
    }

  }
}