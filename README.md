
![Logo](https://i.imgur.com/RtZx8E3.png)


# Arcade Game

Arcade Game is an end of year project of system programming. 
It aims to use the SDL2 libraries and the networking properties available with the C language.

We started by exploring the possibilities of the SDL library by making small games. 

Then in a second time to test the networking features we implemented a client and a server. 

## SDL2

Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D. It is used by video playback software, emulators, and popular games including Valve's award winning catalog and many Humble Bundle games.

### Pong

![App Screenshot]()

First, we need to include those libraries in order to have access to SDL2 and some useful tools.
```c
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
```

Then, for the initilisation of our interface we have a function "init" that executes this code:
```c
//Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		game.state = QUIT;
	}
	TTF_Init();

	font24 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 24);
	font32 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 32);
	font40 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 40);
	font64 = TTF_OpenFont("ressources/fonts/bit5x3.ttf", 64);

	//get screen size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	width = dm.w;
	height = dm.h;

    //Create a window
	window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	//allow resize window
	SDL_SetWindowResizable(window, SDL_TRUE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Sound initialization 
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		game.state = QUIT;
	}
	Mix_AllocateChannels(2);

	//load wav
	sound_bounce = Mix_LoadWAV_RW(SDL_RWFromFile("ressources/SoundEffects/bounce.wav","rb"), 1);
	if(!sound_bounce) {
		printf("Mix_LoadWAV Error : %s\n", Mix_GetError());
		game.state = QUIT;
	}
	sound_menu = Mix_LoadWAV_RW(SDL_RWFromFile("ressources/SoundEffects/menu.wav","rb"), 1);
	if(!sound_menu) {
		printf("Mix_LoadWAV Error : %s\n", Mix_GetError());
		game.state = QUIT;
	}
	sound_bounce_wall = Mix_LoadWAV_RW(SDL_RWFromFile("ressources/SoundEffects/bounce_wall.wav","rb"), 1);
	if(!sound_bounce_wall) {
		printf("Mix_LoadWAV Error : %s\n", Mix_GetError());
		game.state = QUIT;
	}
	sound_point = Mix_LoadWAV_RW(SDL_RWFromFile("ressources/SoundEffects/point.wav","rb"), 1);
	if(!sound_point) {
		printf("Mix_LoadWAV Error : %s\n", Mix_GetError());
		game.state = QUIT;
	}
	sound_endgame = Mix_LoadWAV_RW(SDL_RWFromFile("ressources/SoundEffects/endgame.wav","rb"), 1);
	if(!sound_endgame) {
		printf("Mix_LoadWAV Error : %s\n", Mix_GetError());
		game.state = QUIT;
	}

    init_players();
    init_ball();
```

To update the ball speed and position we have a function called "update_ball" which just check ball position in order to multiply x or/and y speed by -1 to simulate a bounce on a wall or on the player rectangle.

To handle the events, we use a switch condition with multiple state condition:
```c
void event_handler() {
	SDL_Event event;
	//event management
	if (SDL_PollEvent(&event)) {
       	switch (event.type) {
           	case SDL_QUIT:
               	break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					//get screen size
					SDL_DisplayMode dm;
					SDL_GetCurrentDisplayMode(0, &dm);
					width = dm.w;
					height = dm.h;
					width = event.window.data1;
					height = event.window.data2;
					hrect = height/4;
					player1.y = height/2 - hrect/2;
					player2.y = height/2 - hrect/2;
					player1.x = wrect*8;
					player2.x = width - wrect*8;
					ballparam.ix = width/2;
					ballparam.iy = height/2;
					ball.x = ballparam.ix;
					ball.y = ballparam.iy;
				}
				break;		
			case SDL_KEYDOWN:
				if(game.state == ONLINEMENU) {
					// save key in string
					char key[2];
					key[0] = event.key.keysym.sym;
					// if key semicolon
					if(key[0] == ';') {
						key[0] = '.';
					} else if (key[0] == SDLK_BACKSPACE) {
						key[0] = ' ';
						if(i1>4 && !iporsocket) {
							i1--;
							game.ip[i1+1] = '\0';
						}
						if(i2>6 && iporsocket) {
							i2--;
							game.port[i2+1] = '\0';
						}
					} else if (key[0] == SDLK_TAB) {
						key[0] = ' ';
						iporsocket = (iporsocket + 1) % 2;
					} else if(key[0] == SDLK_RETURN) {
						key[0] = ' ';
					}
					if(key[0] != SDLK_ESCAPE) {
						if(i1<19 && !iporsocket && key[0] != ' ') game.ip[i1] = key[0];
						if(i2<13 && iporsocket && key[0] != ' ') game.port[i2] = key[0];
						if(key[0] != ' ' && i1<19 && !iporsocket) i1++;
						if(key[0] != ' ' && i2<13 && iporsocket) i2++;
					}
				}
				switch (event.key.keysym.sym) {
					case SDLK_1:
						if (game.state == OPTIONS) {
							if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
								SDL_SetWindowFullscreen(window, 0);
							} else {
								SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
							}
						} else if(game.state == GAMEMODE) {
							game.state = LOCAL;
						}
						break;
					case SDLK_2:
						if(game.state == GAMEMODE) {
							game.state = ONLINEMENU;
						}
						break;
					// Echap
					case SDLK_ESCAPE:
						if(game.state == MENU) {
							game.state = QUIT;
						} else if(game.state == LOCAL) {
							Mix_PlayChannel(-1, sound_endgame, 0);
							game.state = END;
							game.goal = 0;
						} else if(game.state == ONLINEMENU) {
							Mix_PlayChannel(-1, sound_menu, 0);
							for(int i=4; i<19; i++) game.ip[i] = ' ';
							game.state = GAMEMODE;
						} else if(game.state == OPTIONS || game.state == GAMEMODE) {
							Mix_PlayChannel(-1, sound_menu, 0);
							game.state = MENU;
						}
						break;
					// Espace
					case SDLK_SPACE:
						if(game.state == MENU) {
							Mix_PlayChannel(-1, sound_menu, 0);
							game.state = OPTIONS;
							}	
						break;
					// Enter
					case SDLK_RETURN:
						if (game.state == MENU) {
							Mix_PlayChannel(-1, sound_menu, 0);
							game.state = GAMEMODE;
						} else if(game.state == ONLINEMENU) {
							Mix_PlayChannel(-1, sound_menu, 0);
							game.state = LOCAL;
						}
						break;
					// Player 1
					case SDLK_z:
						if(player1.y > -hrect/2) player1.y -= pspeed;
						break;
					case SDLK_s:
						if(player1.y < height-hrect/2) player1.y += pspeed;
						break;
					// Player 2
					case SDLK_UP:
						if(player2.y > -hrect/2) player2.y -= pspeed;
						break;
					case SDLK_DOWN:
						if(player2.y < height-hrect/2) player2.y += pspeed;
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
```

To render the menu, we determined a succession of lines to get the perfect rendering:
```c
void render_menu() {
	SDL_Color textColor = {255, 255, 255};
	// render Welcome text
	SDL_Surface *textSurface = TTF_RenderText_Solid(font64, "Welcome to Pong", textColor);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Rect textRect = {(width - textSurface->w) / 2, height / 2 - 5 * textSurface->h, textSurface->w, textSurface->h};
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	// render Start game text
	SDL_Surface *textSurface2 = TTF_RenderText_Solid(font24, "Press enter to start", textColor);
	SDL_Texture *textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
	SDL_Rect textRect2 = {(width - textSurface2->w) / 2, height / 2 - textSurface2->h, textSurface2->w, textSurface2->h};
	SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
	SDL_FreeSurface(textSurface2);
	SDL_DestroyTexture(textTexture2);

	// render options text
	SDL_Surface *textSurface3 = TTF_RenderText_Solid(font24, "Press ESP for options", textColor);
	SDL_Texture *textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
	SDL_Rect textRect3 = {(width - textSurface3->w) / 2, height / 2 + textSurface3->h, textSurface3->w, textSurface3->h};
	SDL_RenderCopy(renderer, textTexture3, NULL, &textRect3);
	SDL_FreeSurface(textSurface3);
	SDL_DestroyTexture(textTexture3);

	// render quit text
	SDL_Surface *textSurface4 = TTF_RenderText_Solid(font24, "Press ESC to quit game", textColor);
	SDL_Texture *textTexture4 = SDL_CreateTextureFromSurface(renderer, textSurface4);
	SDL_Rect textRect4 = {(width - textSurface4->w) / 2, height / 2 + 3 * textSurface4->h, textSurface4->w, textSurface4->h};
	SDL_RenderCopy(renderer, textTexture4, NULL, &textRect4);
	SDL_FreeSurface(textSurface4);
	SDL_DestroyTexture(textTexture4);
}
```

### Puissance 4

![App Screenshot]()

### TicTacToe
TODO
![App Screenshot]()

## NETWORK

### Client

### Server

## Deployment

Clone the project

```bash
  git clone https://github.com/GhislainVentre/ArcadeGame.git
```

Go to the project directory

```bash
  cd ArcadeGame
```

Install dependencies

```bash
  TODO
```

To compile the project

```bash
  make server
  make client
```

Run server and client

```bash
  ./server [port]
  ./client
```

## Authors

- [@Magistis](https://github.com/GhislainVentre)
- [@Maxem](https://github.com/Maxemmm)

## Feedback

If you have any feedback, please reach out to us at maubert@gmail.com or ghislain.ventre@gmail.com
