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

void initGraphics(u8* frameBuffer) {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("borp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
  fb = frameBuffer;
//  sdlFB = (u8*)malloc(160*144*4);
}

void updateGraphics() {
  int pitch;
  SDL_LockTexture(tex, nullptr, (void**)(&sdlFB), &pitch);
  for(u32 i = 0; i < (160*144); i++) {
    sdlFB[i*4 + 0] = 255; // alpha
    sdlFB[i*4 + 1] = fb[i]; // red
    sdlFB[i*4 + 2] = fb[i]; // green
    sdlFB[i*4 + 3] = fb[i]; // blue
  }
  SDL_UnlockTexture(tex);
  SDL_RenderCopy(renderer, tex, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void shutdownGraphics() {
  // todo something to free the texture/renderer
  SDL_DestroyWindow(window);
  SDL_Quit();
}