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

int main() {
  FileLoadData file = loadFile("../pkmn_red.gb");
  //FileLoadData file = loadFile("../pkmn_crystal.gbc");
  initMem(file);
  initVideo();
  resetCpu();
  initGraphics(globalVideoState.frameBuffer);
  updateGraphics();

  checkLogo(&file);

  for(;;) {


    SDL_Event e;
    SDL_PollEvent(&e);
    if(e.type == SDL_QUIT) {
      exit(0);
    }




    u32 startCycles = globalState.cycleCount;
    cpuStep();
    u32 newCycles = globalState.cycleCount - startCycles;
    if(!newCycles) {
      printf("got 0 cycles!\n");
      assert(false);
    }
    stepVideo(newCycles);
  }
}