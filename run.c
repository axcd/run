#include<math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#define PI 3.14159265

const int Window_WIDTH = 720;
const int Window_HEIGHT = 1280;

SDL_Window *window = NULL;
SDL_Renderer *render = NULL;
SDL_Texture *bkgtex = NULL;
SDL_Texture *footex = NULL;
SDL_Texture *joytex = NULL;

SDL_Event event;
SDL_Rect dstrect;
SDL_Rect srcrect;

int x, y, R, r;
int xc, yc, rc;
int xs = 205;
int ys = 0;
int xd = 200;
int yd = 300;
int code = 0;


void setDstrect(int x, int y, int w, int h) {
  dstrect.x = x;
  dstrect.y = y;
  dstrect.w = w;
  dstrect.h = h;
}

void setSrcrect(int x, int y, int w, int h) {
  srcrect.x = x;
  srcrect.y = y;
  srcrect.w = w;
  srcrect.h = h;
}

int Init() {
  R = 180;
  r = 80;

  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow("SDL2",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            Window_WIDTH, Window_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    return 1;
  }

  render = SDL_CreateRenderer(window, -1,
                              SDL_RENDERER_ACCELERATED |
                              SDL_RENDERER_PRESENTVSYNC);

  if (render == NULL) {
    return 1;
  }

  return 0;
}

int load_image() {
	
   SDL_Surface *bkg = NULL;
   SDL_Surface *foo = NULL;
   SDL_Surface *joy = NULL;

  bkg = IMG_Load("resource/background.png");
  if (bkg == NULL) {
    return 1;
  }
  bkgtex = SDL_CreateTextureFromSurface(render, bkg);
  SDL_FreeSurface(bkg);

  foo = IMG_Load("resource/foo.png");
  if (foo == NULL) {
    return 1;
  }
  Uint32 colorkey = SDL_MapRGB(foo->format, 0, 0xFF, 0xFF);
  SDL_SetColorKey(foo, 1, colorkey);
  footex = SDL_CreateTextureFromSurface(render, foo);
  SDL_FreeSurface(foo);

  joy = IMG_Load("resource/joystick.png");
  if (joy == NULL) {
    return 1;
  }
  joytex = SDL_CreateTextureFromSurface(render, joy);
  SDL_FreeSurface(joy);

  return 0;
}

int iskey() {

  if (pow(xc, 2) + pow(yc, 2) > pow(R, 2)) {
    return 0;
  }

  if (pow(xc, 2) + pow(yc, 2) < pow(r, 2)) {
    return 0;
  }

  return 1;
}

int up() {

  if ((1.0 * xc / rc) < cos(PI / 4)) {
    return 0;
  }

  return 1;
}

int down() {

  if ((1.0 * xc / rc) > -cos(PI / 4)) {
    return 0;
  }

  return 1;
}

int left() {

  if ((1.0 * yc / rc) > -cos(PI / 4)) {
    return 0;
  }

  return 1;
}

int right() {

  if ((1.0 * yc / rc) < cos(PI / 4)) {
    return 0;
  }

  return 1;
}

void handle_input() {

  if (event.type == SDL_MOUSEMOTION) {

    if (up()) {
      code = 1;
    }

    if (down()) {
      code = 2;
    }

    if (left()) {
      code = 3;
    }

    if (right()) {
      code = 4;
    }
    
    if (!iskey()) {
      code = 0;
    }

  }

  if (event.type == SDL_MOUSEBUTTONUP) {
    code = 0;
  }
}

void move() {

  if (code == 1 && xd <= 250) {
    xs = 205;
    ys += 64;
    ys = ys % 256;
    xd += 16;
  }

  if (code == 2 && xd >= 0) {
    xs = 0;
    ys += 64;
    ys = ys % 256;
    xd -= 16;
  }

  if (code == 3 && yd >= 0) {
    xs = 0;
    ys += 64;
    ys = ys % 256;
    yd -= 16;
  }

  if (code == 4 && yd <= 1150) {
    xs = 205;
    ys += 64;
    ys = ys % 256;
    yd += 16;
  }

}

void display(int xs, int ys, int xd, int yd) {
	
  SDL_RenderClear(render);
  SDL_RenderCopy(render, bkgtex, NULL, NULL);
  setSrcrect(xs, ys, 205, 64);
  setDstrect(xd, yd, 205, 64);
  SDL_RenderCopy(render, footex, &srcrect, &dstrect);
  setSrcrect(0, 0, 360, 360);
  setDstrect(60, 800, 360, 360);
  SDL_RenderCopy(render, joytex, &srcrect, &dstrect);
  SDL_RenderPresent(render);
  
}

void clean_up() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(render);
  SDL_DestroyTexture(bkgtex);
  SDL_DestroyTexture(footex);
  SDL_DestroyTexture(joytex);
  SDL_Quit();
}

int main(int argc, char **args) {

  int fps = 0;
  int quit = 0;

  if (Init())
    return 1;
  if (load_image())
    return 1;

  while (!quit) {
    fps = SDL_GetTicks();

    display(xs, ys, xd, yd);
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }
      x = event.button.x;
      y = event.button.y;
      xc = x - R - 60;
      yc = y - R - 800;
      rc = sqrt(pow(xc, 2) + pow(yc, 2));
      handle_input();
    }

    move();

    fps = SDL_GetTicks() - fps;
    if (fps < 100) {
      SDL_Delay(100 - fps);
    }
  }

  clean_up();

  return 0;
}