//
// Created by jared on 1/11/19.
//

#include <cstdlib>
#include <assert.h>
#include "video.h"
#include "mem.h"
#include "graphics_display.h"

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

void clearVideo() {
  for(u32 i = 0; i < 160*144; i++) {
    globalVideoState.frameBuffer[i] = 255;
  }
}

void dumpVideo() {
  for(u16 i = 0x8000; i < 0x87ff; i++) {
    if((i%8) == 1) {
      printf("@ 0x%04x: ", i);
    }
    printf("0x%02x ", readByte(i));
    if(!(i%8)) printf("\n");
  }
  printf("\n");
  assert(false);
}

u8 readTile(u16 tileAddr, u8 x, u8 y) {

  //tileAddr = 0x8180;
  assert(x <= 8);
  assert(y <= 8);
  x = (7 - x);
  u16 loAddr = tileAddr + (y*(u16)2);
  u16 hiAddr = loAddr + (u16)1;
  u8 lo = readByte(loAddr);
  u8 hi = readByte(hiAddr);
  u8 loV = (lo >> x) & (u8)1;
  u8 hiV = (hi >> x) & (u8)1;
  u8 result = loV * 120 + hiV * 60;
  return result;
}

void renderLine() {
  u8 lcdc = globalMemState.ioRegs[IO_LCDC];
  bool lcdOn            = (lcdc >> 7) & (u8)1;
  bool windowTileMap    = (lcdc >> 6) & (u8)1;
  bool windowEnable     = (lcdc >> 5) & (u8)1;
  bool tileData         = (lcdc >> 4) & (u8)1;
  bool bgTileMap        = (lcdc >> 3) & (u8)1;
  bool objSize          = (lcdc >> 2) & (u8)1;
  bool objEnable        = (lcdc >> 1) & (u8)1;
  bool bgWinEnable      = (lcdc >> 0) & (u8)1;

  u16 windowMapAddr = (u16)(windowTileMap ? 0x9c00 : 0x9800);
  u16 bgTileMapAddr = (u16)(bgTileMap     ? 0x9c00 : 0x9800);
  u16 tileDataAddr  = (u16)(tileData      ? 0x8000 : 0x8800);


  //printf("RENDERLINE %03d (:en %d :bgWin %d)\n", globalVideoState.line, lcdOn, bgWinEnable);



  if(lcdOn && bgWinEnable) {
    // render background onto framebuffer
    u16 tileMapRowAddr = bgTileMapAddr + 32*((((u16)globalVideoState.line + globalMemState.ioRegs[IO_SCROLLY]) & (u16)255) >> 3);
    u8  tileMapColIdx  = globalMemState.ioRegs[IO_SCROLLX] >> 3;
    u8  yPixOffset     = ((u8)globalVideoState.line + globalMemState.ioRegs[IO_SCROLLY]) & (u8)7;
    u8  xPixOffset     = globalMemState.ioRegs[IO_SCROLLX] & (u8)7;
    u8* linePtr        = globalVideoState.frameBuffer + 160 * globalVideoState.line;
    u16 tileIdx        = readByte(tileMapRowAddr + tileMapColIdx); // todo is this really a u16?
    //printf("read tileIdx %d (%d)\n", tileIdx, bgTileMap);
    assert(!bgTileMap);
    // stupid
    if(bgTileMap && tileIdx < 128) tileIdx += 256;

    for(u8 px = 0; px < 160; px++) {
      *linePtr = readTile(tileDataAddr + 16 * tileIdx, xPixOffset, yPixOffset);
//      if(debug)
//        printf("tile %d, x %d\n", tileIdx, xPixOffset);
      xPixOffset++;
      linePtr++;
      if(xPixOffset == 8) {
        xPixOffset = 0;
        tileMapColIdx = (tileMapColIdx + 1) & 31;
        tileIdx = readByte(tileMapRowAddr + tileMapColIdx);
        if(bgTileMap && tileIdx < 128) tileIdx += 256;
      }

    }
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
        renderLine();
      }
      break;
    case 0: // hblank
      if(globalVideoState.modeClock >= 204) {
        globalVideoState.modeClock = 0;
        globalVideoState.line++;

        if(globalVideoState.line == 143) {
          globalVideoState.mode = 1; // vblank
          globalMemState.ioRegs[IO_IF] |= 0x1;
          updateGraphics();
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