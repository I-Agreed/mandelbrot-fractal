#include <SDL2/SDL.h>
#include <algorithm>
using namespace std;

const int WIDTH = 1040;
const int HEIGHT = 500;
const int loops = 100;
SDL_Window* window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                         WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

int startMouseX = 0;
int startMouseY = 0;
int offsetX = 0;
int offsetY = 0;
int zoom = 1;
bool mouseDown = false;

void handle_point(int x, int y) {
    float scaledX = (x - WIDTH*5.0/7.0 - offsetX)*max(3.5/WIDTH, 2.0/HEIGHT)/zoom;
    float scaledY = (y - HEIGHT/2.0 - offsetY)*max(3.5/WIDTH, 2.0/HEIGHT)/zoom;
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
    float l = loops;
    SDL_SetRenderDrawColor(renderer, 255-i/l*255, 255-i/l*255, 255-i/l*255, SDL_ALPHA_OPAQUE);
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

void handle_mouse_movement(SDL_Event event) {
    if (mouseDown) {
        offsetX = event.button.x - startMouseX;
        offsetY = event.button.y - startMouseY;
    }
}

void handle_mouse_button(SDL_Event event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        if (!mouseDown) {
            startMouseX = event.button.x - offsetX;
            startMouseY = event.button.y - offsetY;
        }
        if (event.button.type == SDL_MOUSEBUTTONDOWN) {
            mouseDown = true;
        } else if (event.button.type == SDL_MOUSEBUTTONUP) {
            mouseDown = false;
        }
    }
}

void handle_scroll(SDL_Event event) {
    if (event.wheel.which == SDL_TOUCH_MOUSEID) {
        zoom += event.wheel.y;
    }
    
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    while (true) {
        render_screen();
        SDL_Event event;
        while(SDL_PollEvent(&event) > 0) {
            switch (event.type) {
            case SDL_QUIT: return 0;
            
            case SDL_MOUSEMOTION: handle_mouse_movement(event);
            case SDL_MOUSEBUTTONDOWN: handle_mouse_button(event);
            case SDL_MOUSEBUTTONUP: handle_mouse_button(event);
            case SDL_MOUSEWHEEL: handle_scroll(event);
            }
        }
    }
}