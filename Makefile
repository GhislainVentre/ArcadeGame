game: pong puis4

compile:
	gcc -Wall -g3 -fsanitize=address -pthread server.c -o server
	gcc -Wall -g3 -fsanitize=address -pthread client.c -o client

pong: pong.c
	gcc -o pong -I/usr/include/SDL2 pong.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 -pthread

puis4: puis4.c
	gcc -o puis4 -I/usr/include/SDL2 puis4.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

clear:
	rm -f puis4 pong