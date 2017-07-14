#pragma once

#ifndef MCIGRAPH_H
#define MCIGRAPH_H

#include <iostream>
#include <SDL.h>

#include <thread>
#include <string>
#include <vector>
#include <unordered_map>

using std::string;

namespace mcigraph {

struct MciGraphException {
  std::string message;
  MciGraphException(std::string m) {
    std::cout << "MciGraphException: " << m << std::endl;
    message = m;
  }
};

/// Allows to load images from files and returns a texture and also
/// caches already loaded images. Warning: The cache does not delete
/// already loaded textures etc.
class TextureLoadCache {
private:
  std::unordered_map<std::string, SDL_Texture *> _cache;
  SDL_Renderer *_ren;

public:
  TextureLoadCache() : _ren{NULL} {};
  TextureLoadCache(SDL_Renderer *ren) : _ren{ren} {};

  ~TextureLoadCache() {
    for (auto i : _cache) {
      SDL_DestroyTexture(i.second);
    }
  }

  SDL_Texture *load(std::string filename) {
    // If the file is not in cache: Load, make texture, save to cache and return
    if (_cache.count(filename) == 0) {
      SDL_Surface *bmp = SDL_LoadBMP(filename.c_str());
      if (bmp == NULL) {
        throw MciGraphException("Could not load image: " +
                                string(SDL_GetError()) +
                                " Please put your images in the directory: " +
                                string(SDL_GetBasePath()));
      }
      auto magenta = SDL_MapRGB(bmp->format, 0xFF, 0x00, 0xFF);
      SDL_SetColorKey(bmp, SDL_TRUE, magenta);
      SDL_Texture *tex = SDL_CreateTextureFromSurface(_ren, bmp);
      SDL_FreeSurface(bmp);
      if (tex == NULL) {
        throw MciGraphException("Could not create texture: " +
                                string(SDL_GetError()));
      }
      _cache[filename] = tex;
    }
    return _cache[filename];
  }
};

struct Color {
  int red, green, blue;
};

class MciGraph {
private:
  std::thread _event_loop_thread;
  TextureLoadCache _texcache;
  Color _background;
  std::vector<bool> _keystate;

public:
  bool running;
  SDL_Window *win;
  SDL_Renderer *ren;
  int delay;

  MciGraph() {
    // Init some variables
    _background = {0xEF, 0xEF, 0xEF};
    running = true;
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw MciGraphException("Could not init SDL: " + string(SDL_GetError()));
    }

    // Get the window
    win = SDL_CreateWindow("MCI Graph", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_SHOWN);
    if (win == NULL) {
      SDL_Quit();
      throw MciGraphException("Could not create Window" +
                              string(SDL_GetError()));
    }

    // Init the renderer
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);

    if (ren == NULL) {
      SDL_DestroyWindow(win);
      std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
      SDL_Quit();
      throw MciGraphException("Could not create renderer");
    }
    // Init Texture Cache
    _texcache = TextureLoadCache(ren);
    // Init keystates
    _keystate = std::vector<bool>(284); // 284 is the highest key scancode

    delay = 17;
  }

  /// Clears the screen
  void clear() {
    if (SDL_RenderClear(ren) < 0)
      throw MciGraphException(SDL_GetError());
    if (SDL_SetRenderDrawColor(ren, _background.red, _background.green,
                               _background.blue, 0xFF) < 0)
      throw MciGraphException(SDL_GetError());
    if (SDL_RenderFillRect(ren, NULL) < 0)
      throw MciGraphException(SDL_GetError());
  }

  /// Present the screen to user and do some message handling
  void present() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT: {
        running = false;
        break;
      }
      case SDL_KEYDOWN: { // On keydown set keystate to true
        auto ev = reinterpret_cast<SDL_KeyboardEvent *>(&e);
        _keystate.at(ev->keysym.scancode) = true;
        break;
      }
      case SDL_KEYUP: { // On keydown set keystate to false
        auto ev = reinterpret_cast<SDL_KeyboardEvent *>(&e);
        _keystate.at(ev->keysym.scancode) = false;
        break;
      }
      default:
        break;
      }
    }
    SDL_RenderPresent(ren);
    // Prevent that present is being called too often
    SDL_Delay(delay); // Call with about 60Hz
  }

  /// Check if given key is currently pressed
  bool is_pressed(const Uint8 key) {
    SDL_PumpEvents(); // Update Keymap
    auto keymap = SDL_GetKeyboardState(NULL);
    if (keymap == NULL)
      throw MciGraphException(SDL_GetError());
    return keymap[key];
  }

  /// Check if given key was pressed since last checking
  bool was_pressed(const Uint8 key) {
    if (_keystate.at(key) == true) {
      _keystate.at(key) = false;
      return true;
    }
    return false;
  }

  /// Draw a rectangle
  void draw_rect(int x, int y, int width, int height, bool outline = false,
                 int red = 0x00, int green = 0x00, int blue = 0x00) {
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(ren, red, green, blue, SDL_ALPHA_OPAQUE);
    if (outline) {
      SDL_RenderDrawRect(ren, &rect);
    } else {
      SDL_RenderFillRect(ren, &rect);
    }
  }

  /// Draw a line
  void draw_line(int x1, int y1, int x2, int y2, int red = 0x00,
                 int green = 0x00, int blue = 0x00) {
    if (SDL_SetRenderDrawColor(ren, red, green, blue, SDL_ALPHA_OPAQUE) < 0)
      throw MciGraphException(SDL_GetError());
    if (SDL_RenderDrawLine(ren, x1, y1, x2, y2) < 0)
      throw MciGraphException(SDL_GetError());
  }

  /// Draw a point
  void draw_point(int x, int y, int red = 0x00, int green = 0x00,
                  int blue = 0x00) {
    SDL_SetRenderDrawColor(ren, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(ren, x, y);
  }

  /// Draw an image (given as a file on disc) at position x,y. The
  /// loading of the images is cached.
  void draw_image(std::string filename, int x = 0, int y = 0) {
    auto tex = _texcache.load(filename);
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    SDL_Rect dest_rect = {x, y, w, h};
    SDL_RenderCopy(ren, tex, NULL, &dest_rect);
  }

  ~MciGraph() {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
  }
};

} // mcigraph

// Some horrible stuff is going on after here. This is only done to
// make teaching of an introductory course in C++ easier and is
// definitly NOT GOOD PRACTICE. DON'T DO THIS!!

const auto KEY_LEFT = SDL_SCANCODE_LEFT;
const auto KEY_RIGHT = SDL_SCANCODE_RIGHT;
const auto KEY_UP = SDL_SCANCODE_UP;
const auto KEY_DOWN = SDL_SCANCODE_DOWN;

const auto KEY_1 = SDL_SCANCODE_1;
const auto KEY_2 = SDL_SCANCODE_2;
const auto KEY_3 = SDL_SCANCODE_3;
const auto KEY_4 = SDL_SCANCODE_4;
const auto KEY_5 = SDL_SCANCODE_5;
const auto KEY_6 = SDL_SCANCODE_6;
const auto KEY_7 = SDL_SCANCODE_7;
const auto KEY_8 = SDL_SCANCODE_8;
const auto KEY_9 = SDL_SCANCODE_9;
const auto KEY_0 = SDL_SCANCODE_0;

const auto KEY_W = SDL_SCANCODE_W;
const auto KEY_A = SDL_SCANCODE_A;
const auto KEY_S = SDL_SCANCODE_S;
const auto KEY_D = SDL_SCANCODE_D;

const auto KEY_SPACE = SDL_SCANCODE_SPACE;

mcigraph::MciGraph g; // Make a global instance so we can define functions

#define ___MCILOOPSTART___                                                     \
  while (g.running) {                                                          \
    g.clear();

#define ___MCILOOPEND___                                                       \
  g.present();                                                                 \
  SDL_PumpEvents();                                                            \
  }

bool is_pressed(const Uint8 key) { return g.is_pressed(key); }
bool was_pressed(const Uint8 key) { return g.was_pressed(key); }
void draw_rect(int x, int y, int width, int height, bool outline = false,
               int red = 0x00, int green = 0x00, int blue = 0x00) {
  return g.draw_rect(x, y, width, height, outline, red, green, blue);
}
void draw_line(int x1, int y1, int x2, int y2, int red = 0x00, int green = 0x00,
               int blue = 0x00) {
  g.draw_line(x1, y1, x2, y2, red, green, blue);
}
void draw_point(int x, int y, int red = 0x00, int green = 0x00,
                int blue = 0x00) {
  return g.draw_point(x, y, red, green, blue);
}
void draw_image(std::string filename, int x = 0, int y = 0) {
  return g.draw_image(filename, x, y);
}

void set_delay(int delay) { g.delay = delay; }

#endif /* MCIGRAPH_H */

// Compile (Linux):
// g++ -std=c++11 -lpthread test.cpp `sdl2-config --cflags --libs`

// Compile (MacOS X):
// g++ -std=c++11 test.cpp -g -pthread
// -I/Library/Frameworks/SDL2.framework/Headers -framework SDL2 -framework Cocoa
