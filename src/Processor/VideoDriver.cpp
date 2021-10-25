#include "VideoDriver.h"

int initVideo(VideoDriver* video){
    LOG_ASSERT(video != NULL);

    video->data = (colorArg*)calloc(DISPLAY_HEIGHT * DISPLAY_WIDHT, sizeof(colorArg));

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        LOG_MESSAGE_F(FATAL, "SDL_Init() error: %s\n", SDL_GetError());
        return 1;
    }

    video->window = SDL_CreateWindow("Proc display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDHT * PIXEL_SZ, DISPLAY_HEIGHT * PIXEL_SZ, SDL_WINDOW_SHOWN);
    if(video->window == NULL){
        LOG_MESSAGE_F(FATAL, "SDL_CreateWindow() error: %s\n", SDL_GetError());
        return 1;
    }

    video->renderer = SDL_CreateRenderer(video->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(video->renderer == NULL){
        LOG_MESSAGE_F(FATAL, "SDL_CreateRenderer() error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void finishVideo(VideoDriver* video){
    LOG_ASSERT(video != NULL);
    
    SDL_DestroyRenderer(video->renderer);
    video->renderer = NULL;
    
    SDL_DestroyWindow(video->window);
    video->window = NULL;

    SDL_Quit();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void draw(VideoDriver* video){
    LOG_ASSERT(video != NULL);

    SDL_RenderClear(video->renderer);

    SDL_Rect rect = {};
    rect.h = PIXEL_SZ;
    rect.w = PIXEL_SZ;

    for(int y = 0; y < DISPLAY_HEIGHT; ++y){
        for(int x = 0; x < DISPLAY_WIDHT; ++x){
            colorArg curCol = video->data[DISPLAY_WIDHT * y + x];
            SDL_SetRenderDrawColor(video->renderer, curCol.color.r, curCol.color.g, curCol.color.b, curCol.color.a);

            rect.x = PIXEL_SZ * x;
            rect.y = PIXEL_SZ * y;

        #ifdef HOLLOW_RECT
            SDL_RenderDrawRect(video->renderer, &rect);
        #else
            SDL_RenderFillRect(video->renderer, &rect);
        #endif

        }
    }

    SDL_RenderPresent(video->renderer);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

proc_arg_t* getVideoMemPtr(VideoDriver* video, size_t index){
    LOG_ASSERT(video != NULL);
    LOG_ASSERT(index < DISPLAY_HEIGHT * DISPLAY_WIDHT * sizeof(int));

    return (proc_arg_t*)(((char*)video->data) + index);
}