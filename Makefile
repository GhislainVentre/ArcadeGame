arcadeGame: pong puis4 server slidebar

server: server.c client.c
	gcc -Wall -g3 -fsanitize=address -pthread server.c -o server
	gcc -Wall -g3 -fsanitize=address -pthread client.c -o client -I/usr/include/SDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

pong: pong.c
	gcc -o pong -I/usr/include/SDL2 pong.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 -pthread

puis4: puis4.c
	gcc -o puis4 -I/usr/include/SDL2 puis4.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

slidebar: slidebar.c
	gcc -o slidebar -I/usr/include/SDL2 slidebar.c -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2

clear:
	rm -f puis4 pong server slidebar client
