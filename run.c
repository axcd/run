#include<math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define PI 3.14159265

const int Window_WIDTH = 720;
const int Window_HEIGHT = 1280;

SDL_Window * window = NULL;
SDL_Surface *bkg = NULL;
SDL_Surface *foo = NULL;
SDL_Surface *joy = NULL;
SDL_Renderer *render = NULL;
SDL_Texture *tex = NULL;

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
  return;
}

void setSrcrect(int x, int y, int w, int h) {
  srcrect.x = x;
  srcrect.y = y;
  srcrect.w = w;
  srcrect.h = h;
  return;
}

int Init() {
  R = 180;
  r = 70;

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

  bkg = IMG_Load("resource/background.png");
  if (bkg == NULL) {
    return 1;
  }

  foo = IMG_Load("resource/foo.png");
  if (foo == NULL) {
    return 1;
  }
  Uint32 colorkey = SDL_MapRGB(foo->format, 0, 0xFF, 0xFF);
  SDL_SetColorKey(foo, 1, colorkey);

  joy = IMG_Load("resource/joystick.png");
  if (joy == NULL) {
    return 1;
  }

  return 0;
}

int put_image(SDL_Surface * img, SDL_Rect srcrect, SDL_Rect dstrect) {

  tex = SDL_CreateTextureFromSurface(render, img);
  SDL_RenderCopy(render, tex, &srcrect, &dstrect);

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

  setDstrect(0, 0, 720, 1280);
  setSrcrect(0, 0, 720, 1280);
  put_image(bkg, srcrect, dstrect);
  setSrcrect(xs, ys, 205, 64);
  setDstrect(xd, yd, 205, 64);
  put_image(foo, srcrect, dstrect);
  setSrcrect(0, 0, 360, 360);
  setDstrect(60, 800, 360, 360);
  put_image(joy, srcrect, dstrect);

  SDL_RenderPresent(render);
  return;
}

void clean_up() {
  SDL_FreeSurface(bkg);
  SDL_FreeSurface(foo);
  SDL_FreeSurface(joy);
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(render);
  SDL_DestroyTexture(tex);
  SDL_Quit();
  return;
}

int main(int argc, char **args) {

  int fps = 0;
  int quit = 0;

  if (Init())
    return 1;
  if (load_image())
    return 1;

  display(xs, ys, xd, yd);
  while (!quit) {
    fps = SDL_GetTicks();

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
    display(xs, ys, xd, yd);

    fps = SDL_GetTicks() - fps;
    if (fps < 100) {
      SDL_Delay(100 - fps);
    }
  }

  clean_up();

  return 0;
}