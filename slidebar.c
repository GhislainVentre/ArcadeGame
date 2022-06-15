#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void event_handler();

// SDL Window
SDL_Window *window;
SDL_Renderer *renderer;
SDL_DisplayMode dm;

SDL_Rect rect2;

int width, height;
int quit = 1;

int main(int argc, char *argv[])
{
    //Initliaze SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	TTF_Init();

	//get screen size
	SDL_GetCurrentDisplayMode(0, &dm);
	width = dm.w;
	height = dm.h;

    //Create a window
	window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	//allow resize window
	SDL_SetWindowResizable(window, SDL_TRUE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Sound initialization 
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);

    rect2 = (SDL_Rect){width/2-10, height/2-100, 20, 10};

	Mix_AllocateChannels(2);
    while(quit) {

        event_handler();
        //draw background
        SDL_Rect rect = {0, 0, width, height};
        SDL_SetRenderDrawColor(renderer, 120, 59, 141, 255);
        SDL_RenderFillRect(renderer, &rect);

        //draw line
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, width/2, height/2-100, width/2, height/2+100);

        // draw rectangle on the line
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        SDL_RenderFillRect(renderer, &rect2);

        // draw rectangle on the line
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect3 = {rect2.x+rect2.w/4+3, rect2.y, rect2.w/2-6, height/2+101-rect2.y};
        SDL_RenderFillRect(renderer, &rect3);


        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }
    


    return 0;
}

int flag = 0;

void event_handler() {
	SDL_Event event;
	//event management
	if (SDL_PollEvent(&event)) {
       	switch (event.type) {
           	case SDL_QUIT:
               	break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 0;
                }
            break;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT && event.motion.x >= rect2.x && event.motion.x<= rect2.x + rect2.w) {
                    flag=1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT) {
                    flag=0;
                }
                break;
            case SDL_MOUSEMOTION:
                if(flag) {
                    if(event.motion.y >= height/2-102 && event.motion.y <= height/2+100) {
                        rect2.y = event.motion.y;
                    }
                }
                
                break;
			default:
				break;
		}
	}
}