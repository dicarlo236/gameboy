//
// Created by jared on 1/11/19.
//

#include <cstdlib>
#include <assert.h>
#include "video.h"
#include "mem.h"

VideoState globalVideoState;

void initVideo() {
  globalVideoState.mode = 0;
  globalVideoState.modeClock = 0;
  globalVideoState.line = 0;
  globalVideoState.frameBuffer = (u8*)malloc(160*144);

  for(u32 i = 0; i < 160*144; i++) {
    globalVideoState.frameBuffer[i] = 255;
  }
}

void stepVideo(u32 cycles) {
  globalVideoState.modeClock += cycles;
  //printf("case %d clock %d\n", globalVideoState.mode, globalVideoState.modeClock);
  switch(globalVideoState.mode) {
    case 2: // OAM read, scanning

      if(globalVideoState.modeClock >= 80) {
        globalVideoState.modeClock = 0;
        globalVideoState.mode = 3; //  VRAM read, scanning
      }
      break;
    case 3: //  VRAM read, scanning
      if(globalVideoState.modeClock >= 172) {
        globalVideoState.modeClock = 0;
        globalVideoState.mode = 0; // hblank
        // todo write line to buffer
      }
      break;
    case 0: // hblank
      if(globalVideoState.modeClock >= 204) {
        globalVideoState.modeClock = 0;
        globalVideoState.line++;

        if(globalVideoState.line == 143) {
          globalVideoState.mode = 1; // vblank
          // todo update graphics
        } else {
          globalVideoState.mode = 2; // oam
        }
      }
      break;
    case 1: // vblank
      if(globalVideoState.modeClock >= 456) {
        globalVideoState.modeClock = 0;
        globalVideoState.line++;

        if(globalVideoState.line > 153) {
          globalVideoState.mode = 2;
          globalVideoState.line = 0;
        }
      }
      break;
    default:
      assert(false);
  }
  globalMemState.ioRegs[IO_LY] = (u8)globalVideoState.line;
}

void shutdownVideo() {
  free(globalVideoState.frameBuffer);
}