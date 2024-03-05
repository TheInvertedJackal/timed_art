#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <image_holder.h>
#include <art_session.h>

using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 600;

int letsArt(ArtSession);

int main(int argc, char* argv[]){
    //Do a bunch of stuff with loading structs etc.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "error initializing SDL: " << SDL_GetError() << endl;
        return -1;
    }
    letsArt(ArtSession("."));
    return 0;
}

int letsArt(ArtSession art){
    SDL_Window* art_win = SDL_CreateWindow("Timed Art", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
        , WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);
    if(art_win == NULL){
        cout << "Error with window creation";
        return -1;
    }
    
    SDL_Renderer* art_ren = SDL_CreateRenderer(art_win, -1, SDL_RENDERER_ACCELERATED);
    if (art_ren == NULL) {
	    cout << "Error renderer creation";
	    return -1;
    }

    ImagePool pool = ImagePool();
    string**imgs = art.getImages();
    size_t count = art.getTotalImgCount();
    for (size_t i = 0; i < count; i++)
    {
        pool.addImage(new ImageHolder(art_ren, imgs[i], WINDOW_SIZE_X, WINDOW_SIZE_Y));
    }
    
    // string img_path;
    // cin >> img_path;
    // while(cin){
    //     pool.addImage(new ImageHolder(art_ren, img_path, WINDOW_SIZE_X, WINDOW_SIZE_Y));
    //     cin >> img_path;
    // }

    //Yes, ABS file paths do work...
    //pool.addImage(new ImageHolder(art_ren, "C:\\Users\\mcilveend\\OneDrive - Milwaukee School of Engineering\\Pictures\\ArtAndRefs\\RefsFromComputer\\Art Refrences\\0efa7b568f927c63a7d320a9e4b18ba2.jpg", WINDOW_SIZE_X, WINDOW_SIZE_Y));

    ImageHolder * img_holder_test = pool.getFreshImage();

    SDL_Texture * test_tex = img_holder_test->getTexture();

    size_t completedPhotos = 0;

    bool arting = true;
    while(arting == true){
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        arting = false;
                    break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym){
                            case SDLK_ESCAPE | SDLK_TAB:
                                return 2;
                            break;
                            case SDLK_RETURN:
                                //Complete
                                ++completedPhotos;
                            case SDLK_SPACE:
                                //Skip
                                img_holder_test->destoryTexture();
                                img_holder_test = pool.getFreshImage();
                                if(img_holder_test == nullptr){
                                    cerr << "There was an error with the image..." << endl;
                                }
                                test_tex = img_holder_test->getTexture();
                            break;
                    }
                    break;
                }
        }
        SDL_RenderClear(art_ren);
		SDL_RenderCopy(art_ren, test_tex, NULL, NULL);
		SDL_RenderPresent(art_ren);
    }

    img_holder_test->destoryTexture();
    pool.deletePool();
    SDL_DestroyRenderer(art_ren);
    SDL_DestroyWindow(art_win);
    IMG_Quit();
    SDL_Quit();
    art.destroySession();
    return 0;
}
 