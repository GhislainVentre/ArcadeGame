#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void event_handler();

enum state {MENU, SEARCH_MENU, SEARCHING, CONNECTED} state;

// screen size
int width, height;

int selection = 0, quit = 0, iporsocket = 0;
int i1 = 4, i2 = 6, i3 = 4;

char ip[21], port[14], username[20];

// SDL Window
SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font24;
TTF_Font *font32;
TTF_Font *font40;
TTF_Font *font64;

SDL_Surface *textSurface;
SDL_Texture *textTexture;
SDL_Rect textRect;


int main() {

    //Initliaze SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
	}
	TTF_Init();

    //get screen size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	width = dm.w;
	height = dm.h;

	font24 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 24);
	font32 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 32);
	font40 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 40);
	font64 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 64);

	state = MENU;
	ip[0] = 'I';
	ip[1] = 'P';
	ip[2] = ':';
	ip[3] = ' ';
	port[0] = 'P';
	port[1] = 'o';
	port[2] = 'R';
	port[3] = 'T';
	port[4] = ':';
	port[5] = ' ';
	username[0] = 'U';
	username[1] = 'S';
	username[2] = 'N';
	username[3] = ':';
	username[4] = ' ';
	for(int i=8; i<14; i++) port[i] = '\0';
	for(int i=4; i<22; i++) ip[i] = '\0';
	for(int i=5; i<21; i++) username[i] = '\0';

    //Create a window
	window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	//allow resize window
	SDL_SetWindowResizable(window, SDL_TRUE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    while(!quit) {
		event_handler();

        //render background
    	SDL_SetRenderDrawColor(renderer, 228, 193, 131, 255);
		SDL_Rect rect = {0, 0, width, height}; 
		SDL_RenderFillRect(renderer, &rect);
		SDL_Color textColor = {62, 43, 34};

		switch(state) {
			case MENU:
				// render Welcome text
				textSurface = TTF_RenderText_Solid(font64, "Welcome to Arcade Game", textColor);
				textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
				textRect = (SDL_Rect){(width - textSurface->w) / 2, height / 2 - 5 * textSurface->h, textSurface->w, textSurface->h};
				SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
				SDL_FreeSurface(textSurface);
				SDL_DestroyTexture(textTexture);
				break;
			case SEARCH_MENU:
				// render Welcome text
				textSurface = TTF_RenderText_Solid(font64, "Enter the server IP and Port", textColor);
				textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
				textRect = (SDL_Rect){(width - textSurface->w) / 2, height / 2 - 5 * textSurface->h, textSurface->w, textSurface->h};
				SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
				SDL_FreeSurface(textSurface);
				SDL_DestroyTexture(textTexture);

				if(iporsocket == 2) {
					username[i3] = ' ';
					ip[i1] = ' ';
					port[i2] = '_';
				}
				else if(iporsocket == 1) {
					username[i3] = ' ';
					ip[i1] = '_';
					port[i2] = ' ';
				} else {
					username[i3] = '_';
					ip[i1] = ' ';
					port[i2] = ' ';
				}

				SDL_Color textColor = {255, 255, 255};
				SDL_Surface *textSurface4 = TTF_RenderText_Solid(font24, username, textColor);
				SDL_Texture *textTexture4 = SDL_CreateTextureFromSurface(renderer, textSurface4);
				SDL_Rect textRect4 = {(width - textSurface4->w) / 2, height / 2 - 3*textSurface4->h, textSurface4->w, textSurface4->h};
				SDL_Surface *textSurface2 = TTF_RenderText_Solid(font24, ip, textColor);
				SDL_Texture *textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
				SDL_Rect textRect2 = {(width - textSurface2->w) / 2, height / 2 - textSurface2->h, textSurface2->w, textSurface2->h};
				SDL_Surface *textSurface3 = TTF_RenderText_Solid(font24, port, textColor);
				SDL_Texture *textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
				SDL_Rect textRect3 = {(width - textSurface3->w) / 2, height/ 2 + textSurface3->h, textSurface3->w, textSurface3->h};

				// render black rectangle to enter ip and port
				SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
				SDL_Rect rect1 = {(width - textSurface2->w)/2 - 5, height / 2 - textSurface2->h - 5, textSurface2->w+10, textSurface2->h+10};
				SDL_RenderFillRect(renderer, &rect1);
				SDL_Rect rect2 = {(width - textSurface3->w) / 2 - 5, height / 2 + textSurface3->h - 5, textSurface3->w+10, textSurface3->h+10};
				SDL_RenderFillRect(renderer, &rect2);
				SDL_Rect rect3 = {(width - textSurface4->w)/2 - 5, height / 2 - 3*textSurface4->h - 5, textSurface4->w+10, textSurface4->h+10};
				SDL_RenderFillRect(renderer, &rect3);

				SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
				SDL_RenderCopy(renderer, textTexture3, NULL, &textRect3);
				SDL_RenderCopy(renderer, textTexture4, NULL, &textRect4);
				SDL_FreeSurface(textSurface2);
				SDL_DestroyTexture(textTexture2);
				SDL_FreeSurface(textSurface3);
				SDL_DestroyTexture(textTexture3);
				SDL_FreeSurface(textSurface4);
				SDL_DestroyTexture(textTexture4);
				break;
			case SEARCHING:
				break;
			case CONNECTED:
				break;
		}


		SDL_RenderPresent(renderer);
		SDL_Delay(10);
		SDL_RenderClear(renderer);

    }

    
    return 0;
}

void event_handler() {
	SDL_Event event;
	//event management
	if (SDL_PollEvent(&event)) {
       	switch (event.type) {
           	case SDL_QUIT:
               	break;
            case SDL_KEYDOWN:
				if(state == SEARCH_MENU) {
					// save key in string
					char key[2];
					key[0] = event.key.keysym.sym;
					// if key semicolon
					if(key[0] == ';') {
						key[0] = '.';
					} else if (key[0] == SDLK_BACKSPACE) {
						key[0] = ' ';
						if(i1>4 && iporsocket==1) {
							i1--;
							ip[i1+1] = '\0';
						}else if(i2>6 && iporsocket==2) {
							i2--;
							port[i2+1] = '\0';
						} else if(i3>4 && iporsocket==0) {
							i3--;
							username[i3+1] = '\0';
						}
					} else if (key[0] == SDLK_TAB) {
						key[0] = ' ';
						iporsocket = (iporsocket + 1) % 3;
					} else if(key[0] == SDLK_RETURN) {
						key[0] = ' ';
					}
					if(key[0] != SDLK_ESCAPE) {
						if(i1<sizeof(ip)-2 && iporsocket==1 && key[0] != ' ') ip[i1] = key[0];
						if(i2<sizeof(port)-2 && iporsocket==2 && key[0] != ' ') port[i2] = key[0];
						if(i3<sizeof(username)-2 && !iporsocket && key[0] != ' ') username[i3] = key[0];
						if(key[0] != ' ' && i1<sizeof(ip)-2 && iporsocket==1) i1++;
						if(key[0] != ' ' && i2<sizeof(port)-2 && iporsocket==2) i2++;
						if(key[0] != ' ' && i3<sizeof(username)-2 && !iporsocket) i3++;
					}
				}
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
						if(state == MENU) quit = 1;
						else if(state == SEARCH_MENU) state = MENU;
						else if(state == SEARCHING) state = SEARCH_MENU;
						break;
					case SDLK_RETURN:
						if(state == MENU) {
							if(selection == 0) state = SEARCH_MENU;
						} else if(state == SEARCH_MENU) {
							state = SEARCHING;
						}
						break;
					default:
						break;
                } 
            break;
			default:
				break;
		}
	}
}