#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <image_holder.h>
#include <art_session.h>

using namespace std;

const int WINDOW_SIZE_X = 1000, WINDOW_SIZE_Y = 850;

struct report{
    size_t finished = 0;
    size_t skipped = 0;
    bool quit = false;
    bool error = false;
    string error_msg;
};

report letsArt(ArtSession, int, int);

void resizeLoadingBar(SDL_Rect*, double, double, double);


int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cout << "error initializing SDL: " << SDL_GetError() << endl;
        return -1;
    }
    //-1 -> Close Program, -2 -> Error, Other -> # of arts done!
    letsArt(ArtSession("C:\\Users\\mci25\\OneDrive\\Pictures\\DrawingRefs", 5), WINDOW_SIZE_X, WINDOW_SIZE_Y);
    return 0;
}

report letsArt(ArtSession art, int og_window_x, int og_window_y)
{
    report this_session;
    SDL_Window *art_win = SDL_CreateWindow("Timed Art", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, og_window_x, og_window_y, SDL_WINDOW_RESIZABLE);
    if (art_win == NULL)
    {
        this_session.error_msg = "There was an error with window creaton.";
        this_session.error = true;
        return this_session;
    }

    SDL_Renderer *art_ren = SDL_CreateRenderer(art_win, -1, SDL_RENDERER_ACCELERATED);
    if (art_ren == NULL)
    {
        this_session.error_msg = "There was an error with renderer creaton.";
        this_session.error = true;
        return this_session;
    }

    ImagePool pool = ImagePool();
    string ** imgs = art.getImages();
    size_t count = art.getTotalImgCount();
    cout << count << endl;
    for (size_t i = 0; i < count; i++)
    {
        pool.addImage(new ImageHolder(art_ren, imgs[i], og_window_x, og_window_y));
    }

    SDL_Texture * lb_texture = SDL_CreateTexture(art_ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
    SDL_Rect * lb_rect = new SDL_Rect;
    Uint32 * lb_pixel = new Uint32[1];
    lb_pixel[0] = 0xffffffff;
    SDL_UpdateTexture(lb_texture, NULL, lb_pixel, sizeof(Uint32));
    resizeLoadingBar(lb_rect, og_window_x, og_window_y, 1);

    SDL_Texture * lb_back_texture = SDL_CreateTexture(art_ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
    SDL_Rect * lb_back_rect = new SDL_Rect;
    Uint32 * lb_back_pixel = new Uint32[1];
    lb_back_pixel[0] = 0x88888888;
    SDL_UpdateTexture(lb_back_texture, NULL, lb_back_pixel, sizeof(Uint32));
    resizeLoadingBar(lb_back_rect, og_window_x, og_window_y, 1);

    ImageHolder *img_holder = pool.getFreshImage();

    SDL_Texture *tex = img_holder->getTexture();

    SDL_Rect * rect = new SDL_Rect;

    img_holder->normalize(rect);

    size_t completedPhotos = 0;

    size_t maxPhotos = art.getCount();

    clock_t duration = (clock_t) art.getDuration();

    clock_t start_t = clock();

    bool arting = true;

    double current_window_x = og_window_x, current_window_y = og_window_y;

    double current_percent = 1;

    while (arting){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            switch (event.type){
            case SDL_QUIT:
                arting = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                case SDLK_ESCAPE:
                    arting = false;
                    break;
                case SDLK_BACKSPACE:
                    arting = false;
                    break;
                case SDLK_RETURN:
                    // Complete
                    ++completedPhotos;
                case SDLK_SPACE:
                    // Skip
                    img_holder->destoryTexture();
                    img_holder = pool.getFreshImage();
                    img_holder->setWindowXAndY(current_window_x, current_window_y);
                    if (img_holder == nullptr){
                        cerr << "There was an error with the image..." << endl;
                    }
                    tex = img_holder->getTexture();
                    img_holder->normalize(rect);
                    start_t = clock();
                    break;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event){
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_RESIZED:
                    current_window_x = event.window.data1;
                    current_window_y = event.window.data2;
                    img_holder->setWindowXAndY(current_window_x, current_window_y);
                    img_holder->normalize(rect);
                    resizeLoadingBar(lb_rect, current_window_x, current_window_y, current_percent);
                    resizeLoadingBar(lb_back_rect, current_window_x, current_window_y, 1);
                    break;
                }
                break;
            }
        }
        if(duration != 0){
            double secs = (clock() - start_t) / 1000.0;
            if(secs > duration){
                ++completedPhotos;
                img_holder->destoryTexture();
                img_holder = pool.getFreshImage();
                img_holder->setWindowXAndY(current_window_x, current_window_y);
                if (img_holder == nullptr){
                    cerr << "There was an error with the image..." << endl;
                }
                tex = img_holder->getTexture();
                img_holder->normalize(rect);
                start_t = clock();
            } else {
                resizeLoadingBar(lb_rect, current_window_x, current_window_y, secs / duration);
            }
        }
        SDL_RenderClear(art_ren);
        SDL_RenderCopy(art_ren, tex, NULL, rect);
        SDL_RenderCopy(art_ren, lb_back_texture, NULL, lb_back_rect);
        SDL_RenderCopy(art_ren, lb_texture, NULL, lb_rect);
        SDL_RenderPresent(art_ren);

        arting &= (maxPhotos == 0 || completedPhotos < maxPhotos);
    }
    img_holder->destoryTexture();
    pool.deletePool();

    SDL_DestroyTexture(lb_back_texture);
    SDL_DestroyTexture(lb_texture);
    delete[] lb_back_pixel;
    delete[] lb_pixel;

    SDL_DestroyRenderer(art_ren);
    SDL_DestroyWindow(art_win);
    IMG_Quit();
    SDL_Quit();
    art.destroySession();
    delete rect;
    delete lb_rect;
    delete lb_back_rect;
    return this_session;
}

void resizeLoadingBar(SDL_Rect * rect, double win_x, double win_y, double percent){
    rect->h = (int) (win_y * (1 - IMG_RATIO_H) * .25);
    rect->w = (int) (win_x * (IMG_RATIO_W) * percent);
    rect->x = (int) (win_x * (1 - IMG_RATIO_W) * .5);
    rect->y = (int) (win_y * ((1 - IMG_RATIO_H) * .5 + IMG_RATIO_H));
}
