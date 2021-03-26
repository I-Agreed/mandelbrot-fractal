#include <SDL2/SDL.h>
#include <algorithm>
using namespace std;

const int WIDTH = 1040;
const int HEIGHT = 500;
const int loops = 100;
SDL_Window* window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                         WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

int lastMouseX = 0;
int lastMouseY = 0;
double offsetX = 0;
double offsetY = 0;
float zoom = 1;
bool mouseDown = false;
bool gradient = true;

int plotX_to_windowX(double x) {
    x *= zoom;
    x /= max(3.5/WIDTH, 2.0/HEIGHT);
    x += offsetX;
    x += WIDTH*5.0/7.0;
    return x;
}

int plotY_to_windowY(double y) {
    y *= zoom;
    y /= max(3.5/WIDTH, 2.0/HEIGHT);
    y += offsetY;
    y += WIDTH*5.0/7.0;
    return y;
}

double windowX_to_plotX(double x) {
    x -= WIDTH/2.0;
    x /= zoom;
    x -= offsetX;
    x *= max(3.5/WIDTH, 2.0/HEIGHT);
    return x;
}

double windowY_to_plotY(double y) {
    y -= HEIGHT/2.0;
    y /= zoom;
    y -= offsetY;
    y *= max(3.5/WIDTH, 2.0/HEIGHT);
    return y;
}

void handle_point(int x, int y) {
    
    double scaledX = windowX_to_plotX(x);
    double scaledY = windowY_to_plotY(y);
    double x2 = 0;
    double y2 = 0;
    double x3 = 0;
    int i = 0;
    while (i < loops && x2*x2 + y2*y2 <= 4) {
        x3 = x2*x2 - y2*y2 + scaledX;
        y2 = 2*x2*y2 + scaledY;
        x2 = x3;
        i++;
    }
    float l = loops;
    if (gradient) {
        SDL_SetRenderDrawColor(renderer, 255-i/l*255, 255-i/l*255, 255-i/l*255, SDL_ALPHA_OPAQUE);
    } else if (i == loops) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    }
    
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
        offsetX += (event.button.x - lastMouseX)/zoom;
        offsetY += (event.button.y - lastMouseY)/zoom;
        lastMouseX = event.button.x;
        lastMouseY = event.button.y;
    }
}

void handle_mouse_button(SDL_Event event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        if (event.button.type == SDL_MOUSEBUTTONDOWN) {
            mouseDown = true;
            lastMouseX = event.button.x;
            lastMouseY = event.button.y;
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

void handle_keyPress(SDL_Event event){
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_UP: {
            zoom *= 2;
            break;
        }
        case SDL_SCANCODE_DOWN: {
            zoom /= 2;
            break;
        }
        case SDL_SCANCODE_SPACE: {
            gradient = !gradient;
            break;
        }
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
            case SDL_KEYDOWN: handle_keyPress(event);
            }
        }
    }
}