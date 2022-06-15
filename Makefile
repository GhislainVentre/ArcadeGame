arcadeGame: pong puis4 server

pong: pong.c
	gcc -o pong -I/usr/include/SDL2 pong.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 -pthread

puis4: puis4.c
	gcc -o puis4 -I/usr/include/SDL2 puis4.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

slidebar: slidebar.c
	gcc -o slidebar -I/usr/include/SDL2 slidebar.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

server: server.c client
	gcc -o server server.c

client: client.c
	gcc -o client client.c

clear:
	rm -f puis4 pong