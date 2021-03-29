#include <SDL2/SDL.h>
#include <algorithm>
#include <math.h>
#define function mandelbrot
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

float power = 2;
float colourDropOff = 4;
int colourMinimum = 32;
int colours[loops+1][3];

int plotX_to_windowX(double x) {
    x /= max(3.5/WIDTH, 2.0/HEIGHT);
    x += offsetX;
    x *= zoom;
    x += WIDTH*5.0/7.0;
    return x;
}

int plotY_to_windowY(double y) {
    y /= max(3.5/WIDTH, 2.0/HEIGHT);
    y += offsetY;
    y *= zoom;
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

pair<double, double> trihorn(double zx, double zy, double cx, double cy) {
    double outX = zx*zx - zy*zy + cx;
    double outY = -power * zx * zy + cy;
    return {outX, outY};
}

pair<double, double> burningShip(double zx, double zy, double cx, double cy) {
    double outX = zx*zx - zy*zy + cx;
    double outY = abs(power * zx * zy) + cy;
    return {outX, outY};
}

pair<double, double> mandelbrot(double zx, double zy, double cx, double cy) {
    double outX = zx*zx - zy*zy + cx;
    double outY = power * zx * zy + cy;
    return {outX, outY};
}



void handle_point(int x, int y) {
    
    double cx = windowX_to_plotX(x);
    double cy = windowY_to_plotY(y);
    double zx = 0;
    double zy = 0;
    int i = 0;
    while (i < loops && zx*zx + zy*zy <= 4) {
        pair<double, double> newZ = function(zx, zy, cx, cy);
        zx = newZ.first;
        zy = newZ.second;
        i++;
    }
    float l = loops;
    if (gradient) {
        if (i == loops) {
            SDL_SetRenderDrawColor(renderer, 255-i/l*255, 255-i/l*255, 255-i/l*255, SDL_ALPHA_OPAQUE);
        } else {
            SDL_SetRenderDrawColor(renderer, colours[i][0], 0, 0, SDL_ALPHA_OPAQUE);
        }
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
        case SDL_SCANCODE_PAGEUP: {
            zoom *= 2;
            break;
        }
        case SDL_SCANCODE_PAGEDOWN: {
            zoom /= 2;
            break;
        }
        case SDL_SCANCODE_SPACE: {
            gradient = !gradient;
            break;
        }
        case SDL_SCANCODE_LEFT: {
            offsetX += 30/zoom;
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            offsetX -= 30/zoom;
            break;
        }
        case SDL_SCANCODE_UP: {
            offsetY += 30/zoom;
            break;
        }
        case SDL_SCANCODE_DOWN: {
            offsetY -= 30/zoom;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    for (int i = 1; i <= loops; i ++) {
        int r = pow(-i+loops, colourDropOff)/pow(loops, colourDropOff)*(255-colourMinimum);
        colours[i][0] = 255 - r;
    }
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