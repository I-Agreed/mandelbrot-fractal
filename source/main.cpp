#include <SDL2/SDL.h>
#include <algorithm>
using namespace std;

const int WIDTH = 1040;
const int HEIGHT = 500;
const int loops = 1000;
SDL_Window* window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                         WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

void handle_point(int x, int y) {
    float scaledX = (x - WIDTH*5/7)*3.5/WIDTH;
    float scaledY = (y - HEIGHT/2)*2.0/HEIGHT;
    float x2 = 0;
    float y2 = 0;
    float x3 = 0;
    int i = 0;
    while (i < loops && x2*x2 + y2*y2 <= 4) {
        x3 = x2*x2 - y2*y2 + scaledX;
        y2 = 2*x2*y2 + scaledY;
        x2 = x3;
        i++;
    }
    SDL_SetRenderDrawColor(renderer, 255-i/500.0*255, 255-i/500.0*255, 255-i/500.0*255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(renderer, x, y);
}

void render_screen() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            handle_point(x, y);
        }
    }
    
    SDL_RenderPresent(renderer);
}
int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    while (true) {
        render_screen();
        SDL_Event event;
        while(SDL_PollEvent(&event) > 0) {

            if (event.type == SDL_QUIT) {
                return 0;
            }
        }
    }
}