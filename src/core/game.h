#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>

extern int GME_Init(void);
extern void GME_Quit(void);
extern void GME_Start(void);

extern void GME_RetrievePlayers(void);
extern void GME_RetrieveAreas(void);

extern void GME_BuildRandMap(void);

#endif /* _GAME_H */