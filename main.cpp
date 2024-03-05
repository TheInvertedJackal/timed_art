#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 600;


int main(int argc, char* argv[]){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "error initializing SDL: " << SDL_GetError() << endl;
    }
    SDL_Window* win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
        , WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);

    bool in_use = true;
    while(in_use = true){
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                    in_use = false;
                    break;
                }
        }
    }
}