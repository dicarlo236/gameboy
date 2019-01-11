//
// Created by jared on 1/11/19.
//

#ifndef GBC_VIDEO_H
#define GBC_VIDEO_H

#include "types.h"

struct VideoState {
  u32 mode;
  u32 modeClock;
  u32 line;
  u8* frameBuffer;
};

extern VideoState globalVideoState;

void initVideo();
void stepVideo(u32 cycles);
void shutdownVideo();


#endif //GBC_VIDEO_H
