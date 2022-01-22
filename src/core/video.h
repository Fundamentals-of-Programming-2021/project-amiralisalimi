#ifndef _VIDEO_H
#define _VIDEO_H

#include <SDL2/SDL.h>

extern int VDO_CreateWindow(void);
extern int VDO_CreateRenderer(void);

extern int VDO_Init(void);
extern void VDO_Quit(void);

extern SDL_Window * VDO_GetWindow(void);
extern void VDO_GetWindowSize(int *w, int *h);

#endif /* _VIDEO_H */