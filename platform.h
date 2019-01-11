//
// Created by jared on 1/10/19.
//

#ifndef GBC_PLATFORM_H
#define GBC_PLATFORM_H

#include "types.h"

struct FileLoadData {
  u8* data;
  u32 size;
};

FileLoadData loadFile(const char* name);
void checkLogo(FileLoadData* loadData);

#endif //GBC_PLATFORM_H
