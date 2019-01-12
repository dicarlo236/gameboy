#ifndef GBC_GRAPHICS_DISPLAY_H
#define GBC_GRAPHICS_DISPLAY_H
#include "types.h"

void initGraphics(u8* frameBuffer); // initialize graphics
void updateGraphics();              // draw whatever is in the fb.  format is array of u8's, 160x140, intensity.


#endif //GBC_GRAPHICS_DISPLAY_H
