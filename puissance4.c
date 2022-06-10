#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void event_handler();

struct game {
    enum {
        WAIT,
        GAME,
        QUIT,
        END
    } state;
    int nb_players;
    struct cursor {
        int x;
        int y;
    } cursor1, cursor2;
} game;

SDL_Window *window;
SDL_Renderer *renderer;

int main() {

    SDL_Surface *grille = IMG_Load("ressources/grille.png");

    // Initialize sdl
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    window = SDL_CreateWindow("Puissance 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture * grilles = SDL_CreateTextureFromSurface(renderer, grille);

    while(!game.state == QUIT) {

        event_handler();

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, grilles, NULL, NULL);
        SDL_RenderPresent(renderer);
    }



    return 0;
}

void event_handler() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            game.state = QUIT;
        } else if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                game.state = QUIT;
            } else if(event.key.keysym.sym == SDLK_RETURN) {
                // TODO : play a sound
                // TODO : put a token on the board
            } else if(event.key.keysym.sym == SDLK_RIGHT) {
                if(game.state == GAME) {
                    // TODO : move the cursor to the right
                }
            } else if(event.key.keysym.sym == SDLK_LEFT) {
                if(game.state == GAME && game.cursor2.x > 0) {
                    // TODO : move the cursor to the left
                }
                // TODO : move the cursor to the left
            } 
        }
    }
}