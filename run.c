#include<math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#define PI 3.14159265

int Window_WIDTH = 720;
int Window_HEIGHT = 1280;

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
int xd, yd;
int Max_X, Max_Y;
int code = 0;

int get_data(){

  SDL_DisplayMode displayMode;
  if ( SDL_GetCurrentDisplayMode(0, &displayMode) != 0 ) return 1;
  Window_WIDTH = displayMode.w;
  Window_HEIGHT = displayMode.h;

  R = floor(0.25*Window_WIDTH);
  r = floor(0.12*Window_WIDTH);
  xd = floor(0.06*Window_WIDTH);
  yd = floor(0.06*Window_WIDTH);
  Max_X = floor(0.35*Window_WIDTH);
  Max_Y = floor(0.95*Window_HEIGHT);
	
  srcrect.x = 205;
  srcrect.y = 0;
  srcrect.w = 205;
  srcrect.h = 64;
	
  dstrect.x = floor(0.20*Window_WIDTH);
  dstrect.y = floor(0.45*Window_HEIGHT);
  dstrect.w = 205;
  dstrect.h = 64;
  return 0;
}

int Init() {

  if ( SDL_Init(SDL_INIT_EVERYTHING) != -1 ) return 1;
  if ( get_data() != 0 ) return 1;
  window = SDL_CreateWindow("SDL2 RUN",
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

  if (code == 1 && dstrect.x <= Max_X) {
    srcrect.x = 205;
    srcrect.y += 64;
    srcrect.y = srcrect.y % 256;
    dstrect.x += 16;
  }

  if (code == 2 && dstrect.x >= 0) {
    srcrect.x = 0;
    srcrect.y += 64;
    srcrect.y = srcrect.y % 256;
    dstrect.x -= 16;
  }

  if (code == 3 && dstrect.y >= 0) {
    srcrect.x = 0;
    srcrect.y += 64;
    srcrect.y = srcrect.y % 256;
    dstrect.y -= 16;
  }

  if (code == 4 && dstrect.y <= Max_Y) {
    srcrect.x = 205;
    srcrect.y += 64;
    srcrect.y = srcrect.y % 256;
    dstrect.y += 16;
  }

}

void put_joystick(){
	SDL_Rect drect ;
	SDL_Rect srect ;
	srect.x = 0;
    srect.y = 0;
    srect.w = 2*R;
    srect.h = 2*R;
	drect.x = xd;
    drect.y = yd;
    drect.w = 2*R;
    drect.h = 2*R;
    SDL_RenderCopy(render, joytex, &srect, &drect);
}

void display() {	
  SDL_RenderClear(render);
  SDL_RenderCopy(render, bkgtex, NULL, NULL);
  SDL_RenderCopy(render, footex, &srcrect, &dstrect);
  put_joystick();
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

    display();
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }
      x = event.button.x;
      y = event.button.y;
      xc = x - R - xd;
      yc = y - R - yd;
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