puissance4 : puissance4.c
	gcc -o puissance4 -I/usr/include/SDL2 puissance4.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 -lpthread

clear:
	rm -f pong
