#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void event_handler();
void init();
void addToken();
void render_tokens();
void checkwin();


struct game {
    enum {
        WAIT,
        GAME,
        WIN1,
        WIN2,
        QUIT
    } state;
    int turn;
    int nb_players;
} game;

// SDL Initialization
SDL_Window *window;
SDL_Renderer *renderer;
int width, height;

// SDL_image Initialization
SDL_Rect background;
SDL_Rect grille_rect;
SDL_Rect turn_rect;
SDL_Rect turn_rect_text;
SDL_Rect cursor1_rect;
SDL_Rect cursor2_rect;

SDL_Surface *grille;
SDL_Surface *redtoken;
SDL_Surface *yellowtoken;
SDL_Surface *cursor1;
SDL_Surface *cursor2;

SDL_Texture *grille_texture;
SDL_Texture *turn_texture;
SDL_Texture *cursor1_texture;
SDL_Texture *cursor2_texture;

TTF_Font *font64, *font40;

int tokens[7][6];

int main() {

    init();

    grille = IMG_Load("ressources/grille.png");
    grille_texture = SDL_CreateTextureFromSurface(renderer, grille);

    redtoken = IMG_Load("ressources/redtoken.png");
    turn_texture = SDL_CreateTextureFromSurface(renderer, redtoken);

    cursor1 = IMG_Load("ressources/yellowtoken.png");
    cursor1_texture = SDL_CreateTextureFromSurface(renderer, cursor1);
    cursor2 = IMG_Load("ressources/redtoken.png");
    cursor2_texture = SDL_CreateTextureFromSurface(renderer, cursor2);
    // lower opacity
    SDL_SetTextureAlphaMod(cursor1_texture, 160);
    SDL_SetTextureAlphaMod(cursor2_texture, 160);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_Color textColor = {40, 40, 40};

    //init mp3 music
    Mix_Music *music;
    music = Mix_LoadMUS("ressources/wideputin.mp3");
    Mix_PlayMusic(music, -1);

    // change volume
    Mix_VolumeMusic(MIX_MAX_VOLUME / 10);

    while(game.state != QUIT) {
        event_handler();

        // Render background and grille
		 background = (SDL_Rect){width/2 - grille_rect.w/2, height/2 - grille_rect.h/2, grille_rect.w/2, grille_rect.h/2}; 
        SDL_RenderFillRect(renderer, &background);

        switch(game.state) {
            case WAIT:

                break;
            case GAME:
                // Render cursor1
                if(game.turn == 1) {
                    SDL_SetTextureAlphaMod(cursor1_texture, 160);
                    SDL_RenderCopy(renderer, cursor1_texture, NULL, &cursor1_rect);
                    SDL_SetTextureAlphaMod(cursor1_texture, 255);
                    SDL_RenderCopy(renderer, cursor1_texture, NULL, &turn_rect);
                } else {
                    SDL_SetTextureAlphaMod(cursor2_texture, 160);
                    SDL_RenderCopy(renderer, cursor2_texture, NULL, &cursor2_rect);
                    SDL_SetTextureAlphaMod(cursor2_texture, 255);
                    SDL_RenderCopy(renderer, cursor2_texture, NULL, &turn_rect);
                }

                render_tokens();

                // Render grille
                SDL_RenderCopy(renderer, grille_texture, NULL, &grille_rect);

                // write "Turn :" in turn_rect_text
                SDL_Surface *turn_text = TTF_RenderText_Solid(font40, "Turn :", textColor);
                SDL_Texture *turn_texture = SDL_CreateTextureFromSurface(renderer, turn_text);
                turn_rect_text.w = turn_text->w;
                turn_rect_text.h = turn_text->h;
                turn_rect_text.x = turn_rect.x + turn_rect.w/2 - turn_rect_text.w/2;
                turn_rect_text.y = turn_rect.y - turn_rect.h/2;
                SDL_RenderCopy(renderer, turn_texture, NULL, &turn_rect_text);
                SDL_FreeSurface(turn_text);
                SDL_DestroyTexture(turn_texture);
                break;
            case WIN1:
                // Render grille
                render_tokens();
                SDL_RenderCopy(renderer, grille_texture, NULL, &grille_rect);

                // Render rectangle
                SDL_Rect win_rect = {0, 0, width, height};
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 160);
                SDL_RenderFillRect(renderer, &win_rect);
                
                SDL_Color textColor = {30, 30, 30};
            	// render Welcome text
        	    SDL_Surface *textSurface = TTF_RenderText_Solid(font64, "Yellow Win!!!", textColor);
            	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            	SDL_Rect textRect = {width/2 - textSurface->w/2, height/2 - textSurface->h/2, textSurface->w, textSurface->h};
            	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            	SDL_FreeSurface(textSurface);
            	SDL_DestroyTexture(textTexture);

                break;
            case WIN2:
                // Render grille
                render_tokens();
                SDL_RenderCopy(renderer, grille_texture, NULL, &grille_rect);

                // Render rectangle 
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 20);
                SDL_Rect win_rect2 = {0, 0, width, height};
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 160);
                SDL_RenderFillRect(renderer, &win_rect2);
                // print winner
                SDL_Color textColor1 = {30, 30, 30};
            	SDL_Surface *textSurface2 = TTF_RenderText_Solid(font64, "Red Win !!!", textColor1);
            	SDL_Texture *textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
            	SDL_Rect textRect2 = {width/2 - textSurface2->w/2, height/2 - textSurface2->h, textSurface2->w, textSurface2->h};
            	SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
            	SDL_FreeSurface(textSurface2);
            	SDL_DestroyTexture(textTexture2);
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
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            game.state = QUIT;
        } else if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                game.state = QUIT;
            } else if(event.key.keysym.sym == SDLK_RETURN) {
                if(game.state == GAME) {
                    if(game.turn == 1) {
                        addToken(cursor1_rect.x/(cursor1_rect.w+4));
                    } else {
                        addToken(cursor2_rect.x/(cursor2_rect.w+4));
                    }
                    checkwin();
                }
            } else if(event.key.keysym.sym == SDLK_RIGHT) {
                if(game.state == GAME ) {
                    if(cursor1_rect.x < cursor1_rect.w*6 && game.turn == 1) {
                        // move the cursor to the right
                        cursor1_rect.x += cursor1_rect.w + 4;
                    } else if(cursor2_rect.x < cursor2_rect.w*6 && game.turn == 2) {
                        // move the cursor to the right
                        cursor2_rect.x += cursor2_rect.w + 4;
                    }
                }
            } else if(event.key.keysym.sym == SDLK_LEFT) {
                if(game.state == GAME) {
                    if(cursor1_rect.x > 2 && game.turn == 1) {
                        // move the cursor to the left
                        cursor1_rect.x -= (cursor1_rect.w+4);
                    } else if(cursor2_rect.x > 2 && game.turn == 2) {
                        // move the cursor to the left
                        cursor2_rect.x -= (cursor2_rect.w+4);
                    } 
                }
            } else if(event.type == SDL_MOUSEBUTTONDOWN) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                    if(game.state == GAME) {
                        printf("%d %d\n", event.button.x, event.button.y);
                    }
                }
            }
        }
    }
}

void addToken(int column) {
    // TODO : add a token to the board
    for(int i = 6; i > 0; i--) {
        if(tokens[column][i] == 0) {
            if(game.turn == 1) {
                tokens[column][i] = 1;
                game.turn = 2;
            } else {
                tokens[column][i] = 2;
                game.turn = 1;
            }
            break;
        }
    }
}

void render_tokens() {
    // TODO : render the tokens
    for(int i = 0; i < 7; i++) {
        for(int j = 0; j <= 6; j++) {
            if(tokens[i][j] == 1 && j != 0) {
                SDL_Rect token_rect = {i*(cursor1_rect.w+4)+2, j*(cursor1_rect.h+4), cursor1_rect.w, cursor1_rect.h};
                SDL_SetTextureAlphaMod(cursor1_texture, 255);
                SDL_RenderCopy(renderer, cursor1_texture, NULL, &token_rect);
            } else if(tokens[i][j] == 2 && j != 0) {
                SDL_Rect token_rect = {i*(cursor2_rect.w+4)+2, j*(cursor2_rect.h+4), cursor2_rect.w, cursor2_rect.h};
                SDL_SetTextureAlphaMod(cursor2_texture, 255);
                SDL_RenderCopy(renderer, cursor2_texture, NULL, &token_rect);
            }
        }
    }
}

void checkwin() {
    // Colonne
    for(int i = 0; i < 7; i++) {
        for(int j = 0; j < 4; j++) {
            if(tokens[i][j] == 1 && tokens[i][j+1] == 1 && tokens[i][j+2] == 1 && tokens[i][j+3] == 1) {
                game.state = WIN1;
            } else if(tokens[i][j] == 2 && tokens[i][j+1] == 2 && tokens[i][j+2] == 2 && tokens[i][j+3] == 2) {
                game.state = WIN2;
            }
        }
    }
    // Ligne
    for(int i=0; i<4; i++) {
        for(int j=0; j<7 ; j++) {
            if(tokens[i][j] == 1 && tokens[i+1][j] == 1 && tokens[i+2][j] == 1 && tokens[i+3][j] == 1) {
                game.state = WIN1;
            } else if(tokens[i][j] == 2 && tokens[i+1][j] == 2 && tokens[i+2][j] == 2 && tokens[i+3][j] == 2) {
                game.state = WIN2;
            }

        }
    }    
    // Diagonale
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(tokens[i][j] == 1 && tokens[i+1][j+1] == 1 && tokens[i+2][j+2] == 1 && tokens[i+3][j+3] == 1) {
                game.state = WIN1;
            } else if(tokens[i][j] == 2 && tokens[i+1][j+1] == 2 && tokens[i+2][j+2] == 2 && tokens[i+3][j+3] == 2) {
                game.state = WIN2;
            }
            if(tokens[i][j+3] == 1 && tokens[i+1][j+2] == 1 && tokens[i+2][j+1] == 1 && tokens[i+3][j] == 1) {
                game.state = WIN1;
            } else if(tokens[i][j+3] == 2 && tokens[i+1][j+2] == 2 && tokens[i+2][j+1] == 2 && tokens[i+3][j] == 2) {
                game.state = WIN2;
            }
        }
    }
}

void init() {

    game.state = GAME;
    game.turn = 1;

    //Initliaze SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		game.state = QUIT;
	}
    TTF_Init();
    font64 = TTF_OpenFont("ressources/bigpress.ttf", 64);
    font40 = TTF_OpenFont("ressources/bigpress.ttf", 40);

    //get screen size
	//SDL_DisplayMode dm;
	//SDL_GetCurrentDisplayMode(0, &dm);
	//width = dm.w;
	//height = dm.h;

    width = 673 + 58*2 + 58*1/2;
    height = 576 + 58*8/5;

    grille_rect.w = 673;
    grille_rect.h = 576;
    grille_rect.x = 0;
    grille_rect.y = height - grille_rect.h;

    turn_rect.w = 58*2;
    turn_rect.h = 58*2;
    turn_rect.x = width - turn_rect.w - 58*1/4;
    turn_rect.y = height - turn_rect.h*3/2;

    turn_rect_text.x = turn_rect.x - 58*1/2;
    turn_rect_text.y = turn_rect.y - 58*1/2;

    cursor1_rect.h = 58*8/5;
    cursor1_rect.w = 58*8/5;
    cursor1_rect.x = 2;
    cursor1_rect.y = height - grille_rect.h - cursor1_rect.h;

    cursor2_rect.h = 58*8/5;
    cursor2_rect.w = 58*8/5;
    cursor2_rect.x = 2;
    cursor2_rect.y = height - grille_rect.h - cursor2_rect.h;

    //Create a window
	window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	//allow resize window
	SDL_SetWindowResizable(window, SDL_FALSE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Sound initialization 
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		game.state = QUIT;
	}
	Mix_AllocateChannels(2);
}
