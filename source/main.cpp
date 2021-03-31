#include <CImg.h>
#include <sdl2/SDL.h>
#include <algorithm>
#include <math.h>
#include <string>
#define function mandelbrot
using namespace std;

const int WIDTH = 1040;
const int HEIGHT = 500;
const int IMG_SCALE = 10;
const int loops = 100;
const int imageLoops = 100;
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
int colourMinimum = 64;
int colours[loops+1][3];
int imageColours[imageLoops+1][3];
char filename[] = "out.png";

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

double windowX_to_imageX(double x) {
    x /= IMG_SCALE;
    x -= WIDTH/2.0;
    x /= zoom;
    x -= offsetX;
    x *= max(3.5/WIDTH, 2.0/HEIGHT);
    return x;
}

double windowY_to_imageY(double y) {
    y  /= IMG_SCALE;
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
    if (gradient) {
        SDL_SetRenderDrawColor(renderer, colours[i][0], colours[i][1], colours[i][2], SDL_ALPHA_OPAQUE);
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

void save_image() {
    cimg_library::CImg<unsigned char> img(WIDTH*IMG_SCALE, HEIGHT*IMG_SCALE, 1, 3);
    img.save_png(filename);
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            double cx = windowX_to_imageX(x);
            double cy = windowY_to_imageY(y);
            double zx = 0;
            double zy = 0;
            int i = 0;
            while (i < imageLoops && zx*zx + zy*zy <= 4) {
                pair<double, double> newZ = function(zx, zy, cx, cy);
                zx = newZ.first;
                zy = newZ.second;
                i++;
            }
            for (int j = 0; j < 3; j++) {
                img(x, y, j) = imageColours[i][j];
            }
        }
    }
    img.save_png(filename);
}

void handle_keyPress(SDL_Event event) {
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
        case SDL_SCANCODE_S: {
            save_image();
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < loops; i ++) {
        int r = pow(-i+loops, colourDropOff)/pow(loops, colourDropOff)*(255-colourMinimum);
        colours[i][0] = 255 - r;
    }
    colours[loops][0] = 0;
    colours[loops][1] = 0;
    colours[loops][2] = 0;

    for (int i = 0; i < imageLoops; i ++) {
        int r = pow(-i+imageLoops, colourDropOff)/pow(imageLoops, colourDropOff)*(255-colourMinimum);
        imageColours[i][0] = 255 - r;
    }
    imageColours[imageLoops][0] = 0;
    imageColours[imageLoops][1] = 0;
    imageColours[imageLoops][2] = 0;

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