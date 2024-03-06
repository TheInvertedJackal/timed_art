#include <image_holder.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;

//Holder Def

ImageHolder::ImageHolder(SDL_Renderer* rend,string* path, double x, double y): 
    _path{path},_rend{rend}, _img_in_use{false}{
        setWindowXAndY(x, y);
        srand(time(NULL));
    }

void ImageHolder::setWindowXAndY(double x, double y){
    _screen_x = x;
    _screen_y = y;
}

SDL_Texture * ImageHolder::getTexture(){
    SDL_Surface* sur = IMG_Load(_path->c_str());
    if(sur == NULL){
        cerr << SDL_GetError() << endl;
        return nullptr;
    }
    _img_width = sur->w;
    _img_height = sur->h;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(_rend, sur);
    if(tex == NULL){
        cerr << SDL_GetError() << endl;
        return nullptr;
    }
    SDL_FreeSurface(sur);
    _img_in_use = true;
    _texture = tex;
    return tex;
}

void ImageHolder::normalize(SDL_Rect * fillTo){
    if(!_img_in_use){
        fillTo->x = 0;
        fillTo->y = 0;
        fillTo->w = (int) _screen_x;
        fillTo->h = (int) _screen_y;
    } else {
        double temp_w = _img_width, temp_h = _img_height;
        double bound_w, bound_h;
        bound_w = IMG_RATIO_W * _screen_x;
        bound_h = IMG_RATIO_H * _screen_y;
        temp_w = bound_w;
        double ratio = bound_w / _img_width;
        temp_h *= ratio;
        if(temp_h >= bound_h){
            double ratio = bound_h / temp_h;
            temp_h = bound_h;
            temp_w *= ratio;
        }
        double x_pos = _screen_x * .5, y_pos = _screen_y *.5;
        x_pos -= temp_w * .5;
        y_pos -= temp_h * .5;
        fillTo->x = (int) x_pos;
        fillTo->y = (int) (y_pos - .25 * (_screen_y - temp_h));
        fillTo->w = (int) temp_w;
        fillTo->h = (int) temp_h;
    }
}

void ImageHolder::destoryTexture(){
    if(_img_in_use){
        _img_in_use = false;
        SDL_DestroyTexture(_texture);
    }
}

//Pool Def

ImagePool::ImagePool():
     _useable_count{0}, _used_count{0}, _used_pointer{0}, _used_end{0}, _total_count{0}{}

void ImagePool::addImage(ImageHolder * new_img){
    if(_useable_count < MAX_IMAGES){
        slotUsableImage(new_img);
        ++_total_count;
    }
}

void ImagePool::slotUsableImage(ImageHolder * img){
    _useable_images[_useable_count] = img;
    ++_useable_count;
}

ImageHolder* ImagePool::getFreshImage(){
    if(_useable_count == 0){
        return nullptr;
    }

    int rv = rand() % _useable_count;
    ImageHolder * ret_val = _useable_images[rv];
    --_useable_count;
    if(rv != _useable_count){
        ImageHolder * end_value = _useable_images[_useable_count];
        _useable_images[rv] = end_value;
    }
    _used_images[_used_pointer] = ret_val;
    _used_pointer = (_used_pointer + 1) % MAX_IMAGES;
    ++_used_count;
    if(((double) _used_count) / _total_count >= .6 || _useable_count == 0){
        --_used_count;
        slotUsableImage(_used_images[_used_end]);
        _used_end = (_used_end + 1) % MAX_IMAGES;
    }
    return ret_val;
}

void ImagePool::deletePool(){
    for (int i = 0; i < _useable_count; i++){
        _useable_images[i]->destoryTexture();
        delete _useable_images[i];
    }
    delete[] _useable_images;
    for (int i = _used_end;i != _used_pointer ; i = (i + 1) % MAX_IMAGES){
        _used_images[i]->destoryTexture();
        delete _used_images[i];
    }
    delete[] _used_images;
}