// Linux specific code (excluding graphics, those are in graphics_display.cpp)
#include "platform.h"

#include <stdio.h>
#include <cstdlib>
#include <SDL2/SDL_system.h>
#include <SDL2/SDL_events.h>

// open a file
FileLoadData loadFile(const char* name) {
  FILE* fp = fopen(name, "rb");
  FileLoadData loadData;

  if(!fp) {
    printf("loadFile(%s) failed!\n", name);
    loadData.data = nullptr;
    loadData.size = 0;
    return loadData;
  }

  fseek(fp, 0, SEEK_END);
  u32 fileSize = (u32)ftell(fp);
  fseek(fp, 0, SEEK_SET);
  u8* fileData = (u8*)malloc(fileSize);
  fread(fileData, 1, fileSize, fp);
  fclose(fp);

  printf("loadfile(%s) has loaded %d bytes (%.3f MB)\n", name, fileSize, (float)fileSize / (1 << 20));

  loadData.size = fileSize;
  loadData.data = fileData;
  return loadData;
}

void saveFile(const char* name, FileLoadData info) {
  FILE* fp = fopen(name, "wb");
  printf("save 0x%x bytes 0x%llx\n", info.size, info.data);
  fwrite(info.data, info.size, 1, fp);
  fclose(fp);
}

// update keyboard.  Also checks to see if it's time to quit.
void updateKeyboard(KeyState* keys) {
  SDL_Event e;
  SDL_PollEvent(&e);
  if(e.type == SDL_QUIT) {
    exit(0);
  }
  const u8* keyStats = SDL_GetKeyboardState(nullptr);
  keys->a = keyStats[SDL_SCANCODE_A];
  keys->b = keyStats[SDL_SCANCODE_B];
  keys->u = keyStats[SDL_SCANCODE_UP];
  keys->d = keyStats[SDL_SCANCODE_DOWN];
  keys->l = keyStats[SDL_SCANCODE_LEFT];
  keys->r = keyStats[SDL_SCANCODE_RIGHT];
  keys->save = keyStats[SDL_SCANCODE_S];
  keys->load = keyStats[SDL_SCANCODE_D];
  keys->start = keyStats[SDL_SCANCODE_P];
  keys->select = keyStats[SDL_SCANCODE_L];
  keys->turbo = keyStats[SDL_SCANCODE_SPACE];
}