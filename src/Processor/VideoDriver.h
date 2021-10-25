#ifndef PROC_PROC_VIDEODRIVER_H
#define PROC_PROC_VIDEODRIVER_H

#include "../config.h"
#include "../proc_t.h"
#include "../../lib/Logger.h"

#include <SDL2/SDL.h>


//################### Public ####################################

union colorArg
{
    proc_arg_t value;
    #pragma pack(1)
    struct{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } color;
    #pragma pack()
};


struct VideoDriver{
    colorArg data[DISPLAY_HEIGHT * DISPLAY_WIDHT];

    SDL_Window*   window = NULL;
    SDL_Renderer* renderer = NULL;
};

proc_arg_t* getVideoMemPtr(VideoDriver* video, size_t index);

int initVideo(VideoDriver* video);

void finishVideo(VideoDriver* video);

void draw(VideoDriver* video);


//################# Private #####################################


#endif