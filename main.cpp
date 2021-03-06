#include <SDL2/SDL.h>
#include <stdio.h>
#include "chip8.h"


#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

chip8 myChip8;
int modifier = 10;

int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

bool handleInput(SDL_Event event) {
	int k = -1;
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_x: k = 0; break;
		case SDLK_1: k = 1; break;
		case SDLK_2: k = 2; break;
		case SDLK_3: k = 3; break;
		case SDLK_q: k = 4; break;
		case SDLK_w: k = 5; break;
		case SDLK_e: k = 6; break;
		case SDLK_a: k = 7; break;
		case SDLK_s: k = 8; break;
		case SDLK_d: k = 9; break;
		case SDLK_z: k = 10; break;
		case SDLK_c: k = 11; break;
		case SDLK_4: k = 12; break;
		case SDLK_r: k = 13; break;
		case SDLK_f: k = 14; break;
		case SDLK_v: k = 15; break;
		case SDLK_ESCAPE: return false; break;
		default: break;
		}
		if (k != -1) {
			myChip8.key[k] = 1;	
		}
	} else if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
		case SDLK_x: k = 0; break;
		case SDLK_1: k = 1; break;
		case SDLK_2: k = 2; break;
		case SDLK_3: k = 3; break;
		case SDLK_q: k = 4; break;
		case SDLK_w: k = 5; break;
		case SDLK_e: k = 6; break;
		case SDLK_a: k = 7; break;
		case SDLK_s: k = 8; break;
		case SDLK_d: k = 9; break;
		case SDLK_z: k = 10; break;
		case SDLK_c: k = 11; break;
		case SDLK_4: k = 12; break;
		case SDLK_r: k = 13; break;
		case SDLK_f: k = 14; break;
		case SDLK_v: k = 15; break;
		default: break;
		}
		if (k != -1) {
			myChip8.key[k] = 0;
		}
	}
	return true;
}

void clearScreen(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
	SDL_RenderClear(renderer);
}

void drawPixel(int x, int y, SDL_Renderer *renderer) {
 	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	printf("%d %d\n", x, y);
	SDL_Rect r = {
		x * modifier,
		y * modifier,
		modifier,
		modifier,
	};
	SDL_RenderFillRect(renderer, &r);
}
void gameLoop(SDL_Renderer *renderer) {
	SDL_Event e;
	while(true) {
		myChip8.emulateCycle();
		if (myChip8.drawFlag) {
			clearScreen(renderer);
			for (int x = 0; x < 2048; x++) {
				if (myChip8.gfx[x]) {
					drawPixel(x % 64, x / 64, renderer);
				}
			}
			SDL_RenderPresent(renderer);
			myChip8.drawFlag = false;
		}
		SDL_PollEvent(&e);
		if(!handleInput(e)) break;;
		SDL_Delay(10);

	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: myChip8.exe game\n\n");
		return 1;
	}

	if (!myChip8.loadGame(argv[1])) return 1;
	SDL_Window *window;
	SDL_Renderer *renderer;
 	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
       		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        	return 1;
   	 } else {
		SDL_CreateWindowAndRenderer(display_width, display_height, 0, &window, &renderer); 
	 }
	gameLoop(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
