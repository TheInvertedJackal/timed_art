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
    string error_msg = "";
};

struct session_request{
    string dir = ".";
    size_t duration = 0;
    size_t img_count = 0;
};

session_request * startOut(int, int);

report * letsArt(ArtSession, int, int);

session_request * artAgain(session_request*, report*, int, int);

void resizeLoadingBar(SDL_Rect*, double, double, double);

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cout << "error initializing SDL: " << SDL_GetError() << endl;
        return -1;
    }
    //session_request * new_session = startOut(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    session_request * new_session = new session_request; new_session->dir = "C:\\Users\\mci25\\OneDrive\\Pictures\\DrawingRefs";
    while(new_session != nullptr){
        ArtSession session = ArtSession(new_session->dir, new_session->duration, new_session->img_count);
        //Save dir (and photo count!)and allow for easy access later.
        report * session_report = letsArt(session, WINDOW_SIZE_X, WINDOW_SIZE_Y);
        if(!session_report->quit){
            new_session = artAgain(new_session,session_report, WINDOW_SIZE_X, WINDOW_SIZE_Y);
        } else {
            delete new_session;
            new_session = nullptr;
        }
        delete session_report;
    }
    return 0;
}

session_request * startOut(int win_x, int win_y){
    return nullptr;
}

session_request * artAgain(session_request * p_session, report * s_r, int win_x, int win_y){
    session_request * ret_val = nullptr;
    if(s_r->error){
        cout << "ERROR: " << s_r->error_msg << endl;
    }
    cout << "Completed: " << s_r->finished << endl;
    cout << "Skipped: " << s_r->skipped << endl;
    cout << "Total: " << s_r->finished + s_r->skipped << endl;
    if(!s_r->quit){
        ret_val = p_session;
    }
    return ret_val;
}

report * letsArt(ArtSession art, int og_window_x, int og_window_y)
{
    report * this_session = new report;
    SDL_Window *art_win = SDL_CreateWindow("Timed Art", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, og_window_x, og_window_y, SDL_WINDOW_RESIZABLE);
    if (art_win == NULL)
    {
        this_session->error_msg = SDL_GetError();
        this_session->error = true;
        return this_session;
    }

    SDL_Renderer *art_ren = SDL_CreateRenderer(art_win, -1, SDL_RENDERER_ACCELERATED);
    if (art_ren == NULL)
    {
        SDL_DestroyWindow(art_win);
        this_session->error_msg = SDL_GetError();
        this_session->error = true;
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
                    this_session->quit = true;
                    arting = false;
                    break;
                case SDLK_BACKSPACE:
                    arting = false;
                    break;
                case SDLK_RETURN:
                    // Complete
                    ++this_session->finished;
                case SDLK_SPACE:
                    // Skip
                    ++this_session->skipped;
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
                ++this_session->finished;
                ++this_session->skipped;
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
        if(!art.isInfinite()){
            SDL_RenderCopy(art_ren, lb_back_texture, NULL, lb_back_rect);
            SDL_RenderCopy(art_ren, lb_texture, NULL, lb_rect);
        }
        SDL_RenderPresent(art_ren);

        arting &= (maxPhotos == 0 || completedPhotos < maxPhotos);
    }
    //Clean Up
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

    this_session->skipped -= this_session->finished;
    return this_session;
}

void resizeLoadingBar(SDL_Rect * rect, double win_x, double win_y, double percent){
    rect->h = (int) (win_y * (1 - IMG_RATIO_H) * .25);
    rect->w = (int) (win_x * (IMG_RATIO_W) * percent);
    rect->x = (int) (win_x * (1 - IMG_RATIO_W) * .5);
    rect->y = (int) (win_y * ((1 - IMG_RATIO_H) * .5 + IMG_RATIO_H));
}
