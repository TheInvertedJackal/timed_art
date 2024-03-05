#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <image_holder.h>

using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 600;


int main(int argc, char* argv[]){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "error initializing SDL: " << SDL_GetError() << endl;
        return 1;
    }
    SDL_Window* win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
        , WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);
    if(win == NULL){
        cout << "Error with window creation";
        return 2;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
	    cout << "Error renderer creation";
	    return 3;
    }

    //Do a buch of stuff with loading structs etc.

    //THIS IS A TEST NOT FINAL!
    ImagePool testPool = ImagePool();
    string img_path;
    cin >> img_path;
    while(cin){
        ImageHolder * temp = 
            new ImageHolder(renderer, img_path, WINDOW_SIZE_X, WINDOW_SIZE_Y);
        testPool.addImage(temp);
        cin >> img_path;
    }

    ImageHolder * img_holder_test = testPool.getFreshImage();

    SDL_Texture * test_tex = img_holder_test->getTexture();

    bool in_use = true;
    while(in_use == true){
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        in_use = false;
                    break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym){
                            case SDLK_RETURN:
                            img_holder_test->destoryTexture();
                            img_holder_test = testPool.getFreshImage();
                            test_tex = img_holder_test->getTexture();
                            cout << "SWITCH!" << endl;
                        break;
                    }
                    break;
                }
        }
        SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, test_tex, NULL, NULL);
		SDL_RenderPresent(renderer);
    }

    img_holder_test->destoryTexture();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}