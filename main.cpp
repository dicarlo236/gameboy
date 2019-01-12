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

KeyState keyboard, lastBoard;

int main(int argc, char** argv) {
  if(argc != 2) {
    return 0;
  }
  FileLoadData file = loadFile(argv[1]);
  //FileLoadData file = loadFile("../ttt.gb");
  //FileLoadData file = loadFile("../tetris.gb");
  //FileLoadData file = loadFile("../cpu_instrs.gb");
  //FileLoadData file = loadFile("../opus5.gb");
  //FileLoadData file = loadFile("../test-01.gb"); // "DAA Failed #6"
  //FileLoadData file = loadFile("../test-02.gb"); // "EI Failed #2"
  //FileLoadData file = loadFile("../test-03.gb"); // PASS!
  //FileLoadData file = loadFile("../test-04.gb"); // PASS!
  //FileLoadData file = loadFile("../test-05.gb"); // PASS!
  //FileLoadData file = loadFile("../test-06.gb"); // PASS!
  //FileLoadData file = loadFile("../test-07.gb"); // PASS!
  //FileLoadData file = loadFile("../test-08.gb"); // PASS!
  //FileLoadData file = loadFile("../test-09.gb"); // PASS!
  //FileLoadData file = loadFile("../test-10.gb"); //nyi instructions for this one
  //FileLoadData file = loadFile("../test-11.gb"); //nyi instructions for this one
  initMem(file);
  initVideo();
  resetCpu();
  initGraphics(globalVideoState.frameBuffer);
  updateGraphics();

  checkLogo(&file);
  lastBoard = keyboard;
  for(;;) {


    SDL_Event e;
    SDL_PollEvent(&e);
    updateKeyboard(&keyboard);
    //printf("%d\n", keyboard.toInt());
    if(lastBoard.toInt() != keyboard.toInt()) {
      globalMemState.ioRegs[IO_IF] |= 0x10;
      printf("change keyboard: ime %d, ie 0x%x, if 0x%x\n", globalState.ime, globalMemState.upperRam[0x7f], globalMemState.ioRegs[IO_IF]);
    }

    lastBoard = keyboard;
    if(e.type == SDL_QUIT) {
      exit(0);
    }


    u32 startCycles = globalState.cycleCount;
    cpuStep();
    u32 newCycles = globalState.cycleCount - startCycles;
    if(!newCycles) {
      printf("got 0 cycles!\n");
      printCpuState();
      assert(false);
    }
    stepVideo(newCycles);
  }
}