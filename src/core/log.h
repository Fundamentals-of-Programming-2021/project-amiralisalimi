#ifndef _LOG_H
#define _LOG_H

#define LogInfo(msg) SDL_LogInfo(                     \
    SDL_LOG_CATEGORY_APPLICATION, msg, SDL_GetError() \
)
#define LogError(msg) SDL_LogError(                   \
    SDL_LOG_CATEGORY_APPLICATION, msg, SDL_GetError() \
)

#endif /* _LOG_H */