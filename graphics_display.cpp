//
// Created by jared on 1/10/19.
//

#include <SDL2/SDL.h>
#include "types.h"
static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* tex;
static u8* fb;

static u8* sdlFB;

#define SCALE 4

void initGraphics(u8* frameBuffer) {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("WORM-BOY", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * SCALE, 144 * SCALE, SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, 160 * SCALE, 144 * SCALE);
  fb = frameBuffer;
//  sdlFB = (u8*)malloc(160*144*4);
}

void updateGraphics() {
  int pitch;
  SDL_LockTexture(tex, nullptr, (void**)(&sdlFB), &pitch);
  for(u32 xIn = 0; xIn < 160; xIn++) {
    for(u32 yIn = 0; yIn < 144; yIn++) {
      u32 outCoord = (xIn * SCALE) + (yIn * SCALE * 160 * SCALE);
      u32 inCoord = xIn + yIn * 160;
      for(u32 xScale = 0; xScale < SCALE; xScale++) {
        for(u32 yScale = 0; yScale < SCALE; yScale++) {
          u32 outScaled = outCoord + xScale + (yScale * SCALE * 160);
          sdlFB[outScaled*4 + 0] = 255;
          sdlFB[outScaled*4 + 1] = fb[inCoord];
          sdlFB[outScaled*4 + 2] = fb[inCoord];
          sdlFB[outScaled*4 + 3] = fb[inCoord];
        }
      }
    }
  }
//  for(u32 i = 0; i < (160*144); i++) {
//    sdlFB[(i*4 + 0) * SCALE] = 255; // alpha
//    sdlFB[(i*4 + 1) * SCALE] = fb[i]; // red
//    sdlFB[(i*4 + 2) * SCALE] = fb[i]; // green
//    sdlFB[(i*4 + 3) * SCALE] = fb[i]; // blue
//  }
  SDL_UnlockTexture(tex);
  SDL_RenderCopy(renderer, tex, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void shutdownGraphics() {
  // todo something to free the texture/renderer
  SDL_DestroyWindow(window);
  SDL_Quit();
}