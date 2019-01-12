//
// Created by jared on 1/10/19.
//

#include "platform.h"

#include <stdio.h>
#include <cstdlib>
#include <SDL2/SDL_system.h>

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

void checkLogo(FileLoadData* loadData) {
  u8 sum = 0x19;
  for(u16 i = 0x134; i <= 0x14d; i++)
    sum += loadData->data[i];
  printf("got logo sum 0x%x\n", sum);
}

void updateKeyboard(KeyState* keys) {
  const u8* keyStats = SDL_GetKeyboardState(nullptr);
  keys->a = keyStats[SDL_SCANCODE_A];
  keys->b = keyStats[SDL_SCANCODE_B];
  keys->u = keyStats[SDL_SCANCODE_UP];
  keys->d = keyStats[SDL_SCANCODE_DOWN];
  keys->l = keyStats[SDL_SCANCODE_LEFT];
  keys->r = keyStats[SDL_SCANCODE_RIGHT];
  keys->start = keyStats[SDL_SCANCODE_P];
  keys->select = keyStats[SDL_SCANCODE_L];
  keys->turbo = keyStats[SDL_SCANCODE_SPACE];
}